#include "Walker/Journey/Journey.h"

#include <algorithm>

namespace Walker {
    using namespace Walker::Literals;

	Journey::Journey(OwnedVehicle& vehicle, EndpointKind end, const HomeBase& home) 
		: m_Ps(ServiceLocator::Get().GetRequired<PubSub<Phase>>())
		, m_Vehicle(vehicle)
		, m_End(end)
        , m_Home(home)
	{
        const auto& details = GetEndpointDetails(end);
        m_EndpointCargo = details.InitialCargo;
        m_InitialCargo = details.InitialCargo;
        m_EndpointDistance = details.DistanceFromHome;
        m_UnitCargoWork = details.UnitCargoWork;
	}

    Distance Journey::GetCurrentDistance() const {
        switch(m_Phase) {
            using enum Phase;
            case Outbound: return m_Travel->GetTraveled();
            case Loading: return m_EndpointDistance;
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

	void Journey::Tick(BaseTime elapsed) { 
		if(m_Phase == Phase::Preparing) return;

		m_PendingTime += elapsed;
		while(m_PendingTime >= UpdateInterval) {
            m_PendingTime -= UpdateInterval;

            switch(m_Phase) {
                using enum Phase;
                case Outbound: case Returning: TickTravel(); break;
                case Loading: TickLoading(); break;
                case Unloading: TickUnloading(); break;
            }
		}
	}

	f32 Journey::GetJourneyRatio() const {
		auto result = static_cast<f32>(Distance::Ratio(GetCurrentDistance(), m_EndpointDistance));
        return std::clamp(result, 0.f, 1.f);
	}

	f32 Journey::GetLoadingRatio() const {
        return m_Transfer ? m_Transfer->GetProgress() : 0.f;
    }

    f32 Journey::GetUnloadRatio() const {
        return m_Transfer ? m_Transfer->GetProgress() : 0.f;
    }

    f32 Journey::GetDeliveryRatio() const {
        if(m_InitialCargo <= Zero) return 1.f;

        return std::clamp(static_cast<f32>(Mass::Ratio(m_DeliveredCargo, m_InitialCargo)), 0.f, 1.f);
    }

    f32 Journey::GetEndpointCargoRatio() const {
        if(m_InitialCargo <= Zero) return 1.f;
        return std::clamp(static_cast<f32>(Mass::Ratio(m_EndpointCargo, m_InitialCargo)), 0.f, 1.f);
    }

    Time Journey::GetPhaseEta() const {
        switch(m_Phase) {
            using enum Phase;
            case Loading: return m_Transfer ? m_Transfer->GetEta(m_BaseWorkRate * m_Home.TravelingCrew, m_UnitCargoWork).value_or(Zero) : Zero;
            case Unloading: return m_Transfer ? m_Transfer->GetEta(m_BaseWorkRate * m_Home.TotalCrew, m_UnitCargoWork).value_or(Zero) : Zero;
            case Outbound: // fallthrough
            case Returning: return m_Travel->GetEta(m_Vehicle);
            default: return Zero;
        }

        return Zero;
    }

    void Journey::TickTravel() {
        if(m_Travel->Advance(m_Vehicle, UpdateInterval)) {
			ChangePhase(m_Phase == Phase::Outbound ? Phase::Loading : Phase::Unloading);
        }
    }

	void Journey::TickLoading() {
		auto used = m_Vehicle.CargoMass + m_Vehicle.CrewMass + m_Vehicle.FuelMass;
        auto freeSpace = std::max(Zero, m_Vehicle.TotalCapacity - used);
        auto available = std::min(freeSpace, m_EndpointCargo);
        auto rate = m_BaseWorkRate * m_Home.TravelingCrew;
        auto transferred = m_Transfer->Advance(rate, m_UnitCargoWork, available);

        m_Vehicle.CargoMass += transferred;
        m_EndpointCargo -= transferred;

		if(m_Transfer->Transferred >= m_Transfer->Target || transferred == available) {
            ChangePhase(Phase::Returning);
		}
	}

    void Journey::TickUnloading() {
		auto rate = m_BaseWorkRate * m_Home.TotalCrew;
        auto transferred = m_Transfer->Advance(rate, m_UnitCargoWork, m_Vehicle.CargoMass);
        m_Vehicle.CargoMass -= transferred;
        m_DeliveredCargo += transferred;

        if(m_Vehicle.CargoMass == Zero) {
			ChangePhase(m_EndpointCargo > Zero ? Phase::Preparing : Phase::Complete);
        }
    }

    void Journey::ChangePhase(Phase next) {
		if (next == m_Phase) return;
		if (next == Phase::Preparing || next == Phase::Complete) {
			m_Travel.reset();
			m_Transfer.reset();
		} else if(next == Phase::Outbound || next == Phase::Returning) {
            m_Transfer.reset();
            m_Travel.emplace(m_EndpointDistance, m_ArrivalSpeed);
        } else if(next == Phase::Loading) {
            m_Travel.reset();
            auto used = m_Vehicle.CargoMass + m_Vehicle.CrewMass + m_Vehicle.FuelMass;
            auto freeSpace = std::max(Zero, m_Vehicle.TotalCapacity - used);

            m_Transfer.emplace(CargoTransfer{ .Target = std::min(freeSpace, m_EndpointCargo) });
        } else if(next == Phase::Unloading) {
            m_Travel.reset();
			m_Transfer.emplace(CargoTransfer{ .Target = m_Vehicle.CargoMass });
        }

        m_Phase = next;
        m_Ps.Publish(m_Phase);
    }
}