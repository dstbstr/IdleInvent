#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/WalkerRates.h"

namespace Walker {
    struct OwnedVehicle;

	class TravelLeg {
    public:
        TravelLeg(Distance target, Speed arrivalSpeed, const WalkerRates& rates) 
            : m_TargetDistance(target)
            , m_ArrivalSpeed(arrivalSpeed)
            , m_Rates(rates)
        {}

        bool Advance(OwnedVehicle& vehicle, BaseTime elapsed);
        Distance GetRemainingDistance() const;
        Distance GetTraveled() const { return m_Traveled; }
        Speed GetCurrentSpeed() const { return m_CurrentSpeed; }
        Acceleration GetCurrentAcceleration() const { return m_CurrentAcceleration; }
		Time GetEta(const OwnedVehicle& vehicle) const;

    private:
        Distance m_TargetDistance{};
        Distance m_Traveled{};
        Distance m_DistanceRemainder{}; // stored in micrometers

        Speed m_CurrentSpeed{};
        Speed m_ArrivalSpeed{};
        Acceleration m_CurrentAcceleration{};
        Mass m_FuelConsumed{};
        BaseTime m_PoweredTime{};
        const WalkerRates& m_Rates;

        Time GetRemainingPoweredTime(const OwnedVehicle& vehicle) const;
        struct TravelSegment {
            BaseTime Duration{};
			Acceleration Rate{}; // positive for acceleration, negative for braking
            bool Powered{};
        };

        TravelSegment CalculateNextSegment(const OwnedVehicle& vehicle, BaseTime elapsed) const;
		Distance ApplySegment(OwnedVehicle& vehicle, const TravelSegment& segment);
	};
}