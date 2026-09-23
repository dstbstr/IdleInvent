#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/CargoTransfer.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Travel/TravelLeg.h"
#include "Walker/Travel/Vehicle.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>

#include <optional>

namespace Walker {
    enum struct Phase { Preparing, Outbound, Loading, Returning, Unloading, Complete };
    constexpr std::string_view ToString(Phase phase) {
        switch(phase) {
            using enum Phase;
            case Preparing: return "Preparing";
            case Outbound: return "Outbound";
            case Loading: return "Loading Cargo...";
            case Returning: return "Returning";
            case Unloading: return "Unloading Cargo...";
            case Complete: return "Complete";
        }
        return "Unknown";
    }

	class Journey {
    public:
        Journey(OwnedVehicle* vehicle, EndpointInstance* endpoint, HomeBase& home);

        void Start();
        void Tick(BaseTime elasped);
        f32 GetJourneyRatio() const;
        void ReturnEarly();
		bool ChangeVehicle(OwnedVehicle* vehicle);
		bool ChangeEndpoint(EndpointInstance* end);
		EndpointInstance* GetEndpoint() const { return m_End; }

        Distance GetCurrentDistance() const;
        Distance GetEndDistance() const { return m_End ? m_End->DistanceFromHome : Zero; }
        Speed GetCurrentSpeed() const { return m_Travel ? m_Travel->GetCurrentSpeed() : Zero; }
        Acceleration GetCurrentAcceleration() const { return m_Travel ? m_Travel->GetCurrentAcceleration() : Zero; }
        Phase GetPhase() const { return m_Phase; }
        Mass GetEndpointCargo() const { return m_End ? m_End->RemainingCargo : Zero; }
        Mass GetInitialCargo() const { return m_End ? m_End->InitialCargo : Zero; }
        Mass GetDeliveredCargo() const { return m_End ? m_End->DeliveredCargo : Zero; }

        f32 GetLoadingRatio() const;
        f32 GetUnloadRatio() const;
        f32 GetDeliveryRatio() const;
        f32 GetEndpointCargoRatio() const;

        Time GetPhaseEta() const;
    private:
        PubSub<Phase>& m_Ps;
        OwnedVehicle* m_Vehicle;
        EndpointInstance* m_End{};
        HomeBase& m_Home;
        Phase m_Phase{Phase::Preparing};

        Speed m_ArrivalSpeed{100};


        BaseTime m_PendingTime{};

        std::optional<CargoTransfer> m_Transfer;
        std::optional<TravelLeg> m_Travel;
        WorkRate m_BaseWorkRate{ 1'000 };

        void TickTravel();
        void TickLoading();
        void TickUnloading();
        void ChangePhase(Phase next);
	};
}