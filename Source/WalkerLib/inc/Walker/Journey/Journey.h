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
        }
        return "Unknown";
    }

	class Journey {
    public:
        Journey(OwnedVehicle& vehicle, EndpointKind endpoint);

        void Start();
        void Tick(BaseTime elasped);
        f32 GetJourneyRatio() const;
        Distance GetCurrentDistance() const { return m_CurrentDistance; }
        Distance GetEndDistance() const { return m_EndpointDistance; }
        Speed GetCurrentSpeed() const { return m_CurrentSpeed; }
        Acceleration GetCurrentAcceleration() const { return m_CurrentAccel; }
        Phase GetPhase() const { return m_Phase; }

    private:

        OwnedVehicle& m_Vehicle;
        EndpointKind m_End{};
        Phase m_Phase{Phase::Preparing};

		Distance m_CurrentDistance{0};
        Speed m_CurrentSpeed{0};
        Acceleration m_CurrentAccel{0};

        Distance m_EndpointDistance{0};
		CargoAmount m_EndpointCargo{0};

        BaseTime m_PendingTime{};
        BaseTime m_UpdateInterval{OneSecond};

        PubSub<Phase>& m_Ps;
	};
}