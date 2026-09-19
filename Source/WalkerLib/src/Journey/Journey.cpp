#include "Walker/Journey/Journey.h"

#include <algorithm>

namespace Walker {
    using namespace Walker::Literals;

	Journey::Journey(OwnedVehicle& vehicle, EndpointKind end) 
		: m_Vehicle(vehicle)
		, m_End(end)
		, m_Ps(ServiceLocator::Get().GetRequired<PubSub<Phase>>())
	{
        const auto& details = GetEndpointDetails(end);
        m_EndpointCargo = details.InitialCargo;
        m_InitialCargo = details.InitialCargo;
        m_EndpointDistance = details.DistanceFromHome;
        m_CurrentSpeed = Speed{};
        m_CurrentAccel = vehicle.BaseAcceleration;
        m_UnitCargoWork = details.UnitCargoWork;
	}

	void Journey::Start() {
		if(m_Phase == Phase::Preparing) {
            m_Phase = Phase::Outbound;
            m_Ps.Publish(m_Phase);
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
		auto result = static_cast<f32>(Distance::Ratio(m_CurrentDistance, m_EndpointDistance));
        return std::clamp(result, 0.f, 1.f);
	}

	f32 Journey::GetLoadingRatio() const {
        if(m_LoadTarget <= CargoAmount{}) return 1.f;
        return std::clamp(static_cast<f32>(CargoAmount::Ratio(m_LoadedFromWork, m_LoadTarget)), 0.f, 1.f);
    }

    f32 Journey::GetUnloadRatio() const {
        if(m_UnloadTarget <= CargoAmount{}) return 1.f;
        return std::clamp(static_cast<f32>(CargoAmount::Ratio(m_UnloadedFromWork, m_UnloadTarget)), 0.f, 1.f);
    }

    f32 Journey::GetDeliveryRatio() const {
        if(m_InitialCargo <= CargoAmount{}) return 1.f;

        return std::clamp(static_cast<f32>(CargoAmount::Ratio(m_DeliveredCargo, m_InitialCargo)), 0.f, 1.f);
    }

    f32 Journey::GetEndpointCargoRatio() const {
        if(m_InitialCargo <= CargoAmount{}) return 1.f;
        return std::clamp(static_cast<f32>(CargoAmount::Ratio(m_EndpointCargo, m_InitialCargo)), 0.f, 1.f);
    }

    void Journey::TickTravel() {
        auto previousSpeed = m_CurrentSpeed;
        if(NeedsBrakes()) {
            auto burned = BurnFuel(m_CurrentSpeed - m_ArrivalSpeed);
            ApplyBrakes(burned);
        } else {
            auto burned = BurnFuel(m_Vehicle.MaxSpeed - m_CurrentSpeed);
            ApplyAcceleration(burned);
        }

        auto stepDistance = (previousSpeed + m_CurrentSpeed) * StepMs / (MsPerSec * 2);
        if(m_Phase == Phase::Outbound) {
            m_CurrentDistance += stepDistance;
            if(m_CurrentDistance >= m_EndpointDistance) {
                m_CurrentDistance = m_EndpointDistance;
                m_CurrentSpeed = 0;
                auto used = m_Vehicle.CargoMass + m_Vehicle.CrewMass + m_Vehicle.FuelMass;
                auto freeSpace = std::max(CargoAmount{0}, m_Vehicle.TotalCapacity - used);
                m_LoadTarget = std::min(freeSpace, m_EndpointCargo);
                m_LoadWork = Work{};
                m_LoadedFromWork = CargoAmount{};

                m_Phase = Phase::Loading;
                m_Ps.Publish(m_Phase);
            }
        } else {
            m_CurrentDistance -= std::min(m_CurrentDistance, stepDistance);
            if(m_CurrentDistance == Distance{}) {
                m_CurrentSpeed = 0;
                m_UnloadTarget = m_Vehicle.CargoMass;
                m_UnloadWork = Work{};
                m_UnloadedFromWork = CargoAmount{};

                m_Phase = Phase::Unloading;
                m_Ps.Publish(m_Phase);
            }
        }
    }

	void Journey::TickLoading() {
		auto used = m_Vehicle.CargoMass + m_Vehicle.CrewMass + m_Vehicle.FuelMass;
        auto freeSpace = std::max(CargoAmount{0}, m_Vehicle.TotalCapacity - used);
        auto available = std::min(freeSpace, m_EndpointCargo);

		auto loaded = CargoAmount{};
		if(available > CargoAmount{}) {
			if(m_UnitCargoWork <= Work{}) {
                throw std::domain_error("Cargo work must be positive");
			}

			m_LoadWork += m_LoadRate;
            auto earnedMass = m_LoadWork * 1_Kg / m_UnitCargoWork;
            auto pendingMass = std::max(CargoAmount{}, earnedMass - m_LoadedFromWork);
            loaded = std::min(pendingMass, available);

			m_Vehicle.CargoMass += loaded;
            m_EndpointCargo -= loaded;
            m_LoadedFromWork += loaded;
		}

		if(loaded == available) {
            m_LoadWork = Work{};
            m_LoadedFromWork = CargoAmount{};
            m_CurrentSpeed = Speed{};
            m_Phase = Phase::Returning;
            m_Ps.Publish(m_Phase);
		}
	}

    void Journey::TickUnloading() {
        if(m_Vehicle.CargoMass > CargoAmount{}) {
            if(m_UnitCargoWork <= Work{}) {
                throw std::domain_error("Cargo work must be positive");
            }

            m_UnloadWork += m_UnloadRate;
            auto earnedMass = m_UnloadWork * 1_Kg / m_UnitCargoWork;
            auto pendingMass = std::max(CargoAmount{}, earnedMass - m_UnloadedFromWork);
            auto unloaded = std::min(pendingMass, m_Vehicle.CargoMass);

            m_Vehicle.CargoMass -= unloaded;
            m_UnloadedFromWork += unloaded;
            m_DeliveredCargo += unloaded;
        }

        if(m_Vehicle.CargoMass == CargoAmount{}) {
            m_Phase = m_EndpointCargo > CargoAmount{} ? Phase::Preparing : Phase::Complete;
            m_PendingTime = ZeroTime;
            m_Ps.Publish(m_Phase);
        }
    }

    Distance Journey::GetRemainingDistance() const {
        if(m_Phase == Phase::Outbound) {
            return m_EndpointDistance - m_CurrentDistance;
        } else {
            return m_CurrentDistance;
        }
    }

    bool Journey::NeedsBrakes() const {
        // if we accelerate for another step, will there still be room to brake?
        if(m_CurrentSpeed <= Speed{}) return false;

        auto brakingAccel = m_Vehicle.BaseAcceleration;
        if(brakingAccel <= Acceleration{}) {
            throw std::domain_error("Travel requires positive acceleration");
        }

        auto nextSpeed = std::min(m_CurrentSpeed + m_Vehicle.MaxAcceleration * StepMs / MsPerSec, m_Vehicle.MaxSpeed);

        auto nextStepDistance = nextSpeed * StepMs / MsPerSec;
        auto stoppingDistance = (nextSpeed * nextSpeed - m_ArrivalSpeed * m_ArrivalSpeed) / (brakingAccel * 2);

        return GetRemainingDistance() <= nextStepDistance + stoppingDistance;
    }

    void Journey::ApplyBrakes(BaseTime poweredTime) {
        m_CurrentAccel = Acceleration{};
        if(m_CurrentSpeed <= m_ArrivalSpeed) return;

        poweredTime = std::clamp(poweredTime, ZeroTime, UpdateInterval);

        auto poweredMs = Quantity{poweredTime.count()};
        auto unpoweredMs = StepMs - poweredMs;

        auto weightedAccel = m_Vehicle.MaxAcceleration * poweredMs + m_Vehicle.BaseAcceleration * unpoweredMs;
        auto previousSpeed = m_CurrentSpeed;
        auto delta = weightedAccel / MsPerSec;

        m_CurrentSpeed = std::max(previousSpeed - delta, m_ArrivalSpeed);
        m_CurrentAccel = ((m_CurrentSpeed - previousSpeed) * MsPerSec) / StepMs;
    }

    BaseTime Journey::BurnFuel(Speed requiredChange) {
        if(requiredChange <= Speed{} || m_Vehicle.MaxAcceleration <= Acceleration{}) return ZeroTime;
        if(m_Vehicle.FuelMass <= CargoAmount {} || m_Vehicle.Efficiency <= FuelEfficiency{}) return ZeroTime; 

        auto secondsRequired = Quantity::Ratio(requiredChange, m_Vehicle.MaxAcceleration);
        auto stepSeconds = std::chrono::duration<double>(UpdateInterval).count();
        auto usefulSeconds = std::min(secondsRequired, stepSeconds);
        
        auto usefulTime = std::chrono::duration_cast<BaseTime>(std::chrono::duration<double>{usefulSeconds});

        auto fundedMs = (m_Vehicle.FuelMass + m_FuelConsumed) * m_Vehicle.Efficiency / 1_Kg;
        auto affordableMs = std::clamp(fundedMs - Quantity{m_PoweredTime.count()}, Quantity{}, Quantity{UpdateInterval.count()});
        auto affordableTime = OneInstant * static_cast<BaseTime::rep>(Quantity::Ratio(affordableMs, Quantity{1}));

        auto poweredTime = std::min(usefulTime, affordableTime);
        m_PoweredTime += poweredTime;

        auto totalFuelOwed = Quantity{m_PoweredTime.count()} * 1_Kg / m_Vehicle.Efficiency;
        auto fuelBurned = std::clamp(totalFuelOwed - m_FuelConsumed, CargoAmount{}, m_Vehicle.FuelMass);

        m_Vehicle.FuelMass -= fuelBurned;
        m_FuelConsumed += fuelBurned;

        return poweredTime;
    }

    void Journey::ApplyAcceleration(BaseTime poweredTime) { 
        m_CurrentAccel = Acceleration{};
        if(m_CurrentSpeed >= m_Vehicle.MaxSpeed) return;

        poweredTime = std::clamp(poweredTime, ZeroTime, UpdateInterval);

        auto poweredMs = Quantity{poweredTime.count()};
        auto unpoweredMs = StepMs - poweredMs;

        auto weightedAccel = m_Vehicle.MaxAcceleration * poweredMs + m_Vehicle.BaseAcceleration * unpoweredMs;
        auto previousSpeed = m_CurrentSpeed;
        auto speedIncrease = weightedAccel / MsPerSec;

        m_CurrentSpeed = std::min(previousSpeed + speedIncrease, m_Vehicle.MaxSpeed);
        m_CurrentAccel = ((m_CurrentSpeed - previousSpeed) * MsPerSec) / StepMs;
    }
}