#include "Walker/Journey/Journey.h"

#include <algorithm>

namespace Walker {
    using namespace Walker::Literals;

	Journey::Journey(OwnedVehicle* vehicle, EndpointInstance* end, HomeBase& home) 
		: m_Ps(ServiceLocator::Get().GetRequired<PubSub<PhaseChanged>>())
		, m_Vehicle(vehicle)
		, m_End(end)
        , m_Home(home)
	{
		m_Countdown = m_Home.Rates.GetPreparationDuration();
    }

    Distance Journey::GetCurrentDistance() const {
        switch(m_Phase) {
            using enum Phase;
            case Outbound: return m_Travel->GetTraveled();
            case Loading: return m_End->DistanceFromHome;
            case Returning: return m_Travel->GetRemainingDistance();
            default: return Zero;
        }
    }

	void Journey::Start() {
		if(m_Phase == Phase::Preparing) {
			ChangePhase(Phase::Outbound);
		}
	}

    void Journey::ReturnEarly() {
        if(m_Phase == Phase::Loading) {
			ChangePhase(Phase::Returning);
        }
    }

    bool Journey::ChangeVehicle(OwnedVehicle* vehicle) {
        if(m_Phase != Phase::Preparing || !vehicle) return false;
        m_Vehicle = vehicle;
        return true;
    }

    bool Journey::ChangeEndpoint(EndpointInstance* end) {
		if (m_Phase != Phase::Preparing || !end) return false;
        m_End = end;
        return true;
    }

	void Journey::Tick(BaseTime elapsed) { 
		if(elapsed <= ZeroTime || m_Phase == Phase::Complete) return;

        switch(m_Phase) {
            using enum Phase;
            case Outbound: case Returning: TickTravel(elapsed); break;
            case Loading: TickLoading(elapsed); break;
            case Unloading: TickUnloading(elapsed); break;
            case Preparing:
                if(m_Countdown) {
				    *m_Countdown -= elapsed;
				    if (*m_Countdown <= ZeroTime) Start();
                } else {
					m_Countdown = m_Home.Rates.GetPreparationDuration();
                }
                break;
        }
	}

	f32 Journey::GetJourneyRatio() const {
		auto result = static_cast<f32>(Distance::Ratio(GetCurrentDistance(), m_End->DistanceFromHome));
        return std::clamp(result, 0.f, 1.f);
	}

	f32 Journey::GetLoadingRatio() const {
        return m_Transfer ? m_Transfer->GetProgress() : 0.f;
    }

    f32 Journey::GetUnloadRatio() const {
        return m_Transfer ? m_Transfer->GetProgress() : 0.f;
    }

    f32 Journey::GetDeliveryRatio() const {
        if(m_End->InitialCargo <= Zero) return 1.f;

        return std::clamp(static_cast<f32>(Mass::Ratio(m_End->DeliveredCargo, m_End->InitialCargo)), 0.f, 1.f);
    }

    f32 Journey::GetEndpointCargoRatio() const {
        if(m_End->InitialCargo <= Zero) return 1.f;
        return std::clamp(static_cast<f32>(Mass::Ratio(m_End->RemainingCargo, m_End->InitialCargo)), 0.f, 1.f);
    }

    Time Journey::GetPhaseEta() const {
		auto loadingWorkers = m_Home.Crew[CrewRole::Traveling] + m_End->StationedCrew;
		auto unloadingWorkers = m_Home.Crew.GetCount() - m_Home.Crew[CrewRole::Stationed];
        switch(m_Phase) {
            using enum Phase;
            case Loading: return m_Transfer ? m_Transfer->GetEta(m_Home.Rates.GetCargoWorkRate() * loadingWorkers, m_End->UnitCargoWork).value_or(Zero) : Zero;
            case Unloading: return m_Transfer ? m_Transfer->GetEta(m_Home.Rates.GetCargoWorkRate() * unloadingWorkers, m_End->UnitCargoWork).value_or(Zero) : Zero;
            case Outbound: // fallthrough
            case Returning: return m_Travel->GetEta(*m_Vehicle);
			case Preparing: return m_Countdown ? ToWalkerTime(*m_Countdown) : Zero;
            default: return Zero;
        }

        return Zero;
    }

    void Journey::TickTravel(BaseTime elapsed) {
        auto prevDist = m_Travel->GetTraveled();
		auto prevFuel = m_Vehicle->FuelMass;

		auto arrived = m_Travel->Advance(*m_Vehicle, elapsed);

		m_Home.Stats.OnTravel(m_Travel->GetTraveled() - prevDist, prevFuel - m_Vehicle->FuelMass, m_Vehicle->Kind);
        if(arrived) {
			auto travelers = m_Home.Crew[CrewRole::Traveling] - 1;
            if(m_Phase == Phase::Outbound) {
				ChangePhase(Phase::Loading);
				StationCrew(travelers);
            } else {
				if(m_Home.Crew.TryUnassign(travelers, CrewRole::Traveling)) {
					m_Vehicle->SetCrew(m_Home.Crew[CrewRole::Traveling]);
                }
				ChangePhase(Phase::Unloading);
            }
        }
    }

	void Journey::TickLoading(BaseTime elapsed) {
		auto freeSpace = m_Vehicle->GetAvailableCapacity(m_Home.Rates);
        auto available = std::min(freeSpace, m_End->RemainingCargo);
		auto workers = m_Home.Crew[CrewRole::Traveling] + m_End->StationedCrew;
        auto rate = m_Home.Rates.GetCargoWorkRate() * workers;
        auto numerator = rate * ToWalkerTime(elapsed) + m_WorkRemainder;
        auto work = numerator / MsPerSec;
        m_WorkRemainder = numerator - work * MsPerSec;
        auto transferred = m_Transfer->Advance(work, m_End->UnitCargoWork, available);

        m_Vehicle->CargoMass += transferred;
        m_End->RemainingCargo -= transferred;

		if(m_Transfer->Transferred >= m_Transfer->Target || transferred == available) {
            if(m_End->RemainingCargo == Zero) {
                // TODO: should this be an invention?
				RecoverCrew(m_End->StationedCrew);
            }
            ChangePhase(Phase::Returning);
		}
	}

    void Journey::TickUnloading(BaseTime elapsed) {
        auto workers = m_Home.Crew.GetCount() - m_Home.Crew[CrewRole::Stationed];
		auto rate = m_Home.Rates.GetCargoWorkRate() * workers;
        auto numerator = rate * ToWalkerTime(elapsed) + m_WorkRemainder;
        auto work = numerator / MsPerSec;
        m_WorkRemainder = numerator - work * MsPerSec;

        auto transferred = m_Transfer->Advance(work, m_End->UnitCargoWork, m_Vehicle->CargoMass);
        m_Vehicle->CargoMass -= transferred;
        m_End->DeliveredCargo += transferred;
        m_Home.Funds.Add(transferred);
        m_Home.Stats.OnCargoDelivered(transferred);

        if(m_Vehicle->CargoMass == Zero) {
            m_Home.Stats.OnRoundTripComplete();
            if(m_End->RemainingCargo > Zero || m_End->StationedCrew > 0) {
                ChangePhase(Phase::Preparing);
            } else {
                m_Home.Stats.OnEndpointComplete();
                ChangePhase(Phase::Complete);
            }
        }
    }

    void Journey::StationCrew(u64 count) {
        if(m_Phase != Phase::Loading) return;
		auto toMove = std::min(count, m_Home.Crew[CrewRole::Traveling] - 1);
        if(m_Home.Crew.TryReassign(toMove, CrewRole::Traveling, CrewRole::Stationed)) {
            m_End->StationedCrew += toMove;
			m_Vehicle->SetCrew(m_Home.Crew[CrewRole::Traveling]);

            // dropping off crew allows storing more cargo
            auto available = std::min(m_Vehicle->GetAvailableCapacity(m_Home.Rates), m_End->RemainingCargo);
			m_Transfer->Target = m_Transfer->Transferred + available;
        }
    }

    void Journey::RecoverCrew(u64 count) {
        if(m_Phase != Phase::Loading) return;
		auto toMove = std::min(count, m_End->StationedCrew);
		toMove = std::min(toMove, m_Vehicle->GetRemainingCrewCapacity(m_Home.Rates).TryConvert<u64>().value_or(toMove));
		if (m_Home.Crew.TryReassign(toMove, CrewRole::Stationed, CrewRole::Traveling)) {
			m_End->StationedCrew -= toMove;
            m_Vehicle->SetCrew(m_Home.Crew[CrewRole::Traveling]);
		}
    }

    void Journey::ChangePhase(Phase next) {
		if (next == m_Phase) return;
		if (next == Phase::Preparing || next == Phase::Complete) {
			m_Travel.reset();
			m_Transfer.reset();
            if(next == Phase::Preparing) {
                m_Countdown = m_Home.Rates.GetPreparationDuration();
            }
		} else if(next == Phase::Outbound || next == Phase::Returning) {
            m_Transfer.reset();
            m_Travel.emplace(m_End->DistanceFromHome, m_ArrivalSpeed, m_Home.Rates);
        } else if(next == Phase::Loading) {
            m_Travel.reset();
            auto freeSpace = m_Vehicle->GetAvailableCapacity(m_Home.Rates);
            m_Transfer.emplace(CargoTransfer{ .Target = std::min(freeSpace, m_End->RemainingCargo) });
            m_Home.Stats.OnArrival(m_End->Kind, m_End->DistanceFromHome);
        } else if(next == Phase::Unloading) {
            m_Travel.reset();
			m_Transfer.emplace(CargoTransfer{ .Target = m_Vehicle->CargoMass });
        }

        auto prev = m_Phase;
        m_Phase = next;
        m_Ps.Publish({prev, next});
    }
}