#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Travel/Vehicle.h"
#include <GameState/GameTime.h>

namespace Walker {
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

    private:
        enum struct Phase { Preparing, Outbound, Loading, Returning, Unloading };

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
	};
}