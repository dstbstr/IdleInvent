#include "Walker/Travel/TravelLeg.h"

#include "Walker/Travel/Vehicle.h"

#include <Math/Kinematics.h>

#include <algorithm>
#include <cmath>

namespace Walker {
    bool TravelLeg::Advance(OwnedVehicle& vehicle, BaseTime elapsed) { 
        if(m_Traveled >= m_TargetDistance) return true;
        if(elapsed <= ZeroTime) return false;

        while(elapsed > ZeroTime) {
            auto segment = CalculateNextSegment(vehicle, elapsed);
            auto prevSpeed = m_CurrentSpeed;

            ApplySegment(vehicle, segment);

            auto segmentMs = ToWalkerTime(segment.Duration);
            auto divisor = MsPerSec * 2;
			auto numerator = (prevSpeed + m_CurrentSpeed) * segmentMs + m_DistanceRemainder;

            auto distance = numerator / divisor;
            m_DistanceRemainder = numerator - distance * divisor;
			m_Traveled = std::min(m_Traveled + distance, m_TargetDistance);
            elapsed -= segment.Duration;

            if(m_Traveled >= m_TargetDistance) {
                m_CurrentSpeed = Zero;
                m_CurrentAcceleration = Zero;
                return true;
            }
        }

        return false;
    }

    Distance TravelLeg::GetRemainingDistance() const {
        return m_TargetDistance - m_Traveled;
    }

    Time TravelLeg::GetEta(const OwnedVehicle& vehicle) const {
        auto d = GetRemainingDistance();
        if(d <= Zero) return Zero;

        auto poweredMs = GetRemainingPoweredTime(vehicle);
        auto v = m_CurrentSpeed;
        auto u = m_ArrivalSpeed;
        if(v > u) {
            auto brakingDistance = RequiredDistanceAndTime(vehicle, v, poweredMs).first;
            if(brakingDistance >= d) {
                return 2 * d * MsPerSec / (v + u);
            }
        }

		auto peak = CalculateTargetSpeed(vehicle);
		auto [distance, time] = RequiredDistanceAndTime(vehicle, peak, poweredMs);
		auto coastDistance = std::max(Zero, d - distance);
        return time + coastDistance * MsPerSec / peak;
    }

    std::pair<Distance, Time> TravelLeg::RequiredDistanceAndTime(const OwnedVehicle& vehicle, Speed peak, Time poweredMs) const {
        auto a = vehicle.BaseAcceleration;
		auto b = vehicle.PoweredAcceleration;
        auto v = m_CurrentSpeed;
        auto u = m_ArrivalSpeed;

        auto poweredChange = a * poweredMs / MsPerSec;
        auto poweredIncrease = std::min(peak - v, poweredChange);
        auto accelerationEnd = v + poweredIncrease;
        poweredChange -= poweredIncrease;

        auto poweredDecrease = std::min(peak - u, poweredChange);
        auto brakingEnd = peak - poweredDecrease;

		auto accelerating = 
            DistanceForSpeedChange<Distance>(v, accelerationEnd, a) +
            DistanceForSpeedChange<Distance>(accelerationEnd, peak, b);

		auto braking = 
            DistanceForSpeedChange<Distance>(peak, brakingEnd, a) +
            DistanceForSpeedChange<Distance>(brakingEnd, u, b);

        auto distance = accelerating + braking;

        auto poweredSpeedChange = poweredIncrease + poweredDecrease;
        auto unpoweredSpeedChange = (peak - accelerationEnd) + (brakingEnd - u);

		auto poweredTimeMs = TimeForSpeedChange<Time>(Zero, poweredSpeedChange, a, MsPerSec);
		auto unpoweredTimeMs = TimeForSpeedChange<Time>(Zero, unpoweredSpeedChange, b, MsPerSec);

        return std::make_pair(distance, poweredTimeMs + unpoweredTimeMs);
    }

    Speed TravelLeg::CalculateTargetSpeed(const OwnedVehicle& vehicle) const {
        auto a = vehicle.PoweredAcceleration;
        auto d = GetRemainingDistance();
        auto v = m_CurrentSpeed;
        auto u = m_ArrivalSpeed;

        auto p2 = PeakSpeedSquaredForDistance(d, v, u, a);
        auto poweredTarget = std::min(vehicle.MaxSpeed, p2.Sqrt());
        auto poweredMs = GetRemainingPoweredTime(vehicle);
        auto dA = std::max(Zero, poweredTarget - v);
        auto dB = std::max(Zero, poweredTarget - u);
        if(a * poweredMs >= (dA + dB) * MsPerSec) {
            return poweredTarget;
        }

        auto b = vehicle.BaseAcceleration;
        if(poweredMs == Zero) {
            p2 = PeakSpeedSquaredForDistance(d, v, u, b);
            return std::min(vehicle.MaxSpeed, p2.Sqrt());
        }

		auto low = std::max(v, u);
        auto high = poweredTarget;
        while(true) {
            auto peak = low + (high - low) / 2;
            if(peak <= low || peak >= high) return low;

            if(RequiredDistanceAndTime(vehicle, peak, poweredMs).first <= d) {
				low = peak;
			}
			else {
				high = peak;
            }
        }

        return low;
    }

	Time TravelLeg::GetRemainingPoweredTime(const OwnedVehicle& vehicle) const {
		using namespace Walker::Literals;
		if (vehicle.Efficiency <= Zero || vehicle.FuelMass <= Zero) return Zero;

		auto fundedMs = (vehicle.FuelMass + m_FuelConsumed) * vehicle.Efficiency / 1_Kg;
        return std::max(Zero, fundedMs - Time{ m_PoweredTime.count() });
	}

    TravelLeg::TravelSegment TravelLeg::CalculateNextSegment(const Walker::OwnedVehicle& vehicle, BaseTime elapsed) const {
        auto v = m_CurrentSpeed;
		auto u = m_ArrivalSpeed;

        auto poweredMs = GetRemainingPoweredTime(vehicle);

        auto MakeSpeedChange = [&](Speed delta) -> TravelSegment {
            auto braking = delta < Zero;
            auto dv = braking ? -delta : delta;
			auto powered = poweredMs > Zero;
			auto mag = powered ? vehicle.PoweredAcceleration : vehicle.BaseAcceleration;

            auto durationTime = std::min(ToWalkerTime(elapsed), TimeForSpeedChange<Time>(Zero, dv, mag, MsPerSec));
            if(powered) {
                durationTime = std::min(durationTime, poweredMs);
            }

			auto duration = std::max(OneInstant, ToBaseTime(durationTime));
			auto rate = std::min(mag, dv * MsPerSec / ToWalkerTime(duration));

            return {
				.Duration = duration,
				.Rate = braking ? -rate : rate,
				.Powered = powered
			};
        };


		auto speedToLose = std::max(Zero, v - u);
		auto poweredReduction = std::min(speedToLose, vehicle.PoweredAcceleration * poweredMs / MsPerSec);
        
        // speed post power brakes
		auto intermediateSpeed = v - poweredReduction;

		auto poweredDistance = DistanceForSpeedChange<Distance>(v, intermediateSpeed, vehicle.PoweredAcceleration);
        auto unpoweredDistance = intermediateSpeed > u
			? DistanceForSpeedChange<Distance>(intermediateSpeed, u, vehicle.BaseAcceleration)
            : Zero;

		auto brakingDistance = poweredDistance + unpoweredDistance;
        auto d = GetRemainingDistance();

        // if we need to slow down
        if (v > u && brakingDistance >= d) {
            return MakeSpeedChange(u - v);
        }
        auto targetSpeed = CalculateTargetSpeed(vehicle);
        // if we need to speed up
        if (v < targetSpeed) {
            return MakeSpeedChange(targetSpeed - v);
        }

        // just coast
        auto coastDistance = std::max(Zero, d - brakingDistance);
        auto coastTime = std::min(ToWalkerTime(elapsed), coastDistance * MsPerSec / v);
		auto duration = std::max(OneInstant, ToBaseTime(coastTime));

        return {.Duration = duration, .Rate = Zero, .Powered = false };
    }

    void TravelLeg::ApplySegment(OwnedVehicle& vehicle, const TravelLeg::TravelSegment& segment) {
		using namespace Walker::Literals;

        auto segmentMs = Time{segment.Duration.count()};
        m_CurrentAcceleration = segment.Rate;
        m_CurrentSpeed += segment.Rate * segmentMs / MsPerSec;

        if(segment.Powered) {
            m_PoweredTime += segment.Duration;

            auto totalFuelOwed = Quantity{ m_PoweredTime.count() } * 1_Kg / vehicle.Efficiency;
            auto fuelBurned = std::clamp(totalFuelOwed - m_FuelConsumed, Zero, vehicle.FuelMass);

            vehicle.FuelMass -= fuelBurned;
            m_FuelConsumed += fuelBurned;
        }
    }

}