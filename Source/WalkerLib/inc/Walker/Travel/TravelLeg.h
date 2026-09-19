#pragma once

#include "Walker/WalkerUnits.h"

namespace Walker {
    struct OwnedVehicle;

	class TravelLeg {
    public:
        TravelLeg(Distance target, Speed arrivalSpeed) : m_TargetDistance(target), m_ArrivalSpeed(arrivalSpeed) {}

        bool Advance(OwnedVehicle& vehicle, BaseTime elapsed);
        Distance GetRemainingDistance() const;
        Distance GetTraveled() const { return m_Traveled; }
        Speed GetCurrentSpeed() const { return m_CurrentSpeed; }
        Acceleration GetCurrentAcceleration() const { return m_CurrentAcceleration; }

    private:
        Distance m_TargetDistance{};
        Distance m_Traveled{};

        Speed m_CurrentSpeed{};
        Speed m_ArrivalSpeed{};
        Acceleration m_CurrentAcceleration{};
        Mass m_FuelConsumed{};
        BaseTime m_PoweredTime{};

        BaseTime BurnFuel(OwnedVehicle& vehicle, Speed requiredChange, BaseTime elapsed);
        bool NeedsBrakes(const OwnedVehicle& vehicle, BaseTime elapsed) const;
        void ApplyBrakes(const OwnedVehicle& vehicle, BaseTime poweredTime, BaseTime elapsed);
        void ApplyAcceleration(const OwnedVehicle& vehicle, BaseTime poweredTime, BaseTime elapsed);
	};
}