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
		while(m_PendingTime >= m_UpdateInterval) {
            m_PendingTime -= m_UpdateInterval;

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
        auto burned = BurnFuel();
        ApplyAcceleration(burned);

        if(m_Phase == Phase::Outbound) {
            m_CurrentDistance += m_CurrentSpeed;
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
            m_CurrentDistance -= std::min(m_CurrentDistance, m_CurrentSpeed);
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

    BaseTime Journey::BurnFuel() {
        if(m_CurrentSpeed >= m_Vehicle.MaxSpeed || m_Vehicle.MaxAcceleration <= Acceleration{}) return ZeroTime;
        if(m_Vehicle.FuelMass <= CargoAmount {} || m_Vehicle.Efficiency <= FuelEfficiency{}) return ZeroTime; 

        auto speedGap = m_Vehicle.MaxSpeed - m_CurrentSpeed;
        auto secondsToMax = Quantity::Ratio(speedGap, m_Vehicle.MaxAcceleration);
        auto stepSeconds = std::chrono::duration<double>(m_UpdateInterval).count();
        auto usefulSeconds = std::min(secondsToMax, stepSeconds);
        
        auto usefulTime = std::chrono::duration_cast<BaseTime>(std::chrono::duration<double>{usefulSeconds});

        auto fundedMs = (m_Vehicle.FuelMass + m_FuelConsumed) * m_Vehicle.Efficiency / 1_Kg;
        auto affordableMs = std::clamp(fundedMs - Quantity{m_PoweredTime.count()}, Quantity{}, Quantity{m_UpdateInterval.count()});
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

        poweredTime = std::clamp(poweredTime, ZeroTime, m_UpdateInterval);

        auto stepMs = Quantity{m_UpdateInterval.count()};
        auto poweredMs = Quantity{poweredTime.count()};
        auto unpoweredMs = stepMs - poweredMs;

        auto weightedAccel = m_Vehicle.MaxAcceleration * poweredMs + m_Vehicle.BaseAcceleration * unpoweredMs;
        auto previousSpeed = m_CurrentSpeed;
        auto speedIncrease = weightedAccel / Quantity{OneSecond.count()};

        m_CurrentSpeed = std::min(previousSpeed + speedIncrease, m_Vehicle.MaxSpeed);
        m_CurrentAccel = ((m_CurrentSpeed - previousSpeed) * Quantity{OneSecond.count()}) / stepMs;
    }
}