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
		Time GetEta(const OwnedVehicle& vehicle) const;

    private:
        Distance m_TargetDistance{};
        Distance m_Traveled{};
        Distance m_DistanceRemainder{};

        Speed m_CurrentSpeed{};
        Speed m_ArrivalSpeed{};
        Acceleration m_CurrentAcceleration{};
        Mass m_FuelConsumed{};
        BaseTime m_PoweredTime{};

        Speed CalculateTargetSpeed(const OwnedVehicle& vehicle) const;
        Time GetRemainingPoweredTime(const OwnedVehicle& vehicle) const;
        struct TravelSegment {
            BaseTime Duration{};
			Acceleration Rate{}; // positive for acceleration, negative for braking
            bool Powered{};
        };

		std::pair<Distance, Time> RequiredDistanceAndTime(const OwnedVehicle& vehicle, Speed peak, Time poweredMs) const;
        TravelSegment CalculateNextSegment(const OwnedVehicle& vehicle, BaseTime elapsed) const;
		void ApplySegment(OwnedVehicle& vehicle, const TravelSegment& segment);
	};
}