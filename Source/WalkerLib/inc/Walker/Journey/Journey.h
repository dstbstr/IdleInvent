#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Journey/CargoTransfer.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Journey/ScoutState.h"
#include "Walker/Travel/TravelLeg.h"
#include "Walker/Travel/Vehicle.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>

#include <optional>

namespace Walker {
    struct HomeBase;

    enum struct Phase { Preparing, Outbound, Loading, Returning, Unloading, Complete, Scouting };
    struct PhaseChanged {
        Phase From;
        Phase To;
    };

    constexpr std::string_view ToString(Phase phase) {
        switch(phase) {
            using enum Phase;
            case Preparing: return "Preparing";
            case Outbound: return "Outbound";
            case Loading: return "Loading Cargo...";
            case Returning: return "Returning";
            case Unloading: return "Unloading Cargo...";
            case Complete: return "Complete";
            case Scouting: return "Scouting";
        }
        return "Unknown";
    }

	class Journey {
    public:
        Journey(HomeBase& home);

        void Start();
        void Tick(BaseTime elasped);
        f32 GetJourneyRatio() const;
        void ReturnEarly();
		
        VehicleKind GetVehicleKind() const { return m_Vehicle ? m_Vehicle->Kind : VehicleKind::Unset; }
		bool ChangeVehicle(OwnedVehicle& vehicle);
        
		EndpointKind GetNextScout() const { return m_NextScout; }
		void SetNextScout(EndpointKind kind) { m_NextScout = kind; }
		const EndpointInstance* GetEndpoint() const { return m_End ? &(*m_End) : nullptr; }
		//bool ChangeEndpoint(EndpointInstance* end);

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

        void StationCrew(u64 count);
        void RecoverCrew(u64 count);

        Time GetPhaseEta() const;
        void Reset();

    private:
        PubSub<PhaseChanged>& m_Ps;
        OwnedVehicle* m_Vehicle{nullptr};

        std::optional<EndpointInstance> m_End{};
		EndpointKind m_NextScout{ EndpointKind::Neighborhood };

        HomeBase& m_Home;
        Phase m_Phase{Phase::Preparing};

        Speed m_ArrivalSpeed{100};

        std::optional<BaseTime> m_Countdown{};

        std::optional<ScoutState> m_Scouting{};
        std::optional<CargoTransfer> m_Transfer;
        std::optional<TravelLeg> m_Travel;
        WorkRate m_WorkRemainder{};

        void BeginScouting();
        void TickScout(BaseTime elapsed);
        void TickTravel(BaseTime elapsed);
        void TickLoading(BaseTime elapsed);
        void TickUnloading(BaseTime elapsed);
        void ChangePhase(Phase next);
	};
}