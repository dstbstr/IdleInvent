#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Travel/Vehicle.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>

namespace Walker {
    enum struct Phase { Preparing, Outbound, Loading, Returning, Unloading, Complete };
    constexpr std::string_view ToString(Phase phase) {
        switch(phase) {
            using enum Phase;
            case Preparing: return "Preparing";
            case Outbound: return "Outbound";
            case Loading: return "Loading";
            case Returning: return "Returning";
            case Unloading: return "Unloading";
            case Complete: return "Complete";
        }
        return "Unknown";
    }

	class Journey {
    public:
        Journey(OwnedVehicle& vehicle, EndpointKind endpoint);

        void Start();
        void Tick(BaseTime elasped);
        f32 GetJourneyRatio() const;
        EndpointKind GetEndpoint() const { return m_End; }
        Distance GetCurrentDistance() const { return m_CurrentDistance; }
        Distance GetEndDistance() const { return m_EndpointDistance; }
        Speed GetCurrentSpeed() const { return m_CurrentSpeed; }
        Acceleration GetCurrentAcceleration() const { return m_CurrentAccel; }
        Phase GetPhase() const { return m_Phase; }
        CargoAmount GetEndpointCargo() const { return m_EndpointCargo; }
        CargoAmount GetInitialCargo() const { return m_InitialCargo; }
        CargoAmount GetDeliveredCargo() const { return m_DeliveredCargo; }

        f32 GetLoadingRatio() const;
        f32 GetUnloadRatio() const;
        f32 GetDeliveryRatio() const;
        f32 GetEndpointCargoRatio() const;
    private:

        OwnedVehicle& m_Vehicle;
        EndpointKind m_End{};
        Phase m_Phase{Phase::Preparing};

		Distance m_CurrentDistance{0};
        Speed m_CurrentSpeed{0};
        Speed m_ArrivalSpeed{100};
        Acceleration m_CurrentAccel{0};

        Distance m_EndpointDistance{0};
		CargoAmount m_EndpointCargo{0};

        BaseTime m_PendingTime{};
        BaseTime m_PoweredTime{};
        CargoAmount m_FuelConsumed{};

        Work m_UnitCargoWork{}; // Work per Kg
        WorkRate m_LoadRate{1'000}; // TODO: Get from crew
        WorkRate m_UnloadRate{2'000}; // TODO: Get from crew
        Work m_LoadWork{};
        Work m_UnloadWork{};

        CargoAmount m_InitialCargo{};
        CargoAmount m_DeliveredCargo{};

        CargoAmount m_LoadedFromWork{};
        CargoAmount m_UnloadedFromWork{};
        CargoAmount m_LoadTarget{};
        CargoAmount m_UnloadTarget{};

        void TickTravel();
        void TickLoading();
        void TickUnloading();

        Distance GetRemainingDistance() const;
        bool NeedsBrakes() const;
        void ApplyBrakes(BaseTime poweredTime);

        BaseTime BurnFuel(Speed requiredChange);
        void ApplyAcceleration(BaseTime poweredTime);

        PubSub<Phase>& m_Ps;
	};
}