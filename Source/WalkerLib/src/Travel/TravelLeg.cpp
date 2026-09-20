#include "Walker/Travel/TravelLeg.h"

#include "Walker/Travel/Vehicle.h"

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
        auto a = vehicle.MaxAcceleration;
		auto b = vehicle.BaseAcceleration;
        auto v = m_CurrentSpeed;
        auto u = m_ArrivalSpeed;
        auto v2 = v * v;
        auto u2 = u * u;

        auto poweredChange = a * poweredMs / MsPerSec;
        auto poweredIncrease = std::min(peak - v, poweredChange);
        auto accelerationEnd = v + poweredIncrease;
        poweredChange -= poweredIncrease;

        auto poweredDecrease = std::min(peak - u, poweredChange);
        auto brakingEnd = peak - poweredDecrease;
        auto p2 = peak * peak;
        auto a2 = accelerationEnd * accelerationEnd;
        auto b2 = brakingEnd * brakingEnd;

        auto accelerating = (a2 - v2) / (a * 2) + (p2 - a2) / (2 * b);
        auto braking = (p2 - b2) / (a * 2) + (b2 - u * u) / (2 * b);

        auto distance = accelerating + braking;

        auto poweredSpeedChange = poweredIncrease + poweredDecrease;
        auto unpoweredSpeedChange = (peak - accelerationEnd) + (brakingEnd - u);

        auto poweredTimeMs = poweredSpeedChange * MsPerSec / a;
        auto unpoweredTimeMs = unpoweredSpeedChange * MsPerSec / vehicle.BaseAcceleration;

        return std::make_pair(distance, poweredTimeMs + unpoweredTimeMs);
    }

    Speed TravelLeg::CalculateTargetSpeed(const OwnedVehicle& vehicle) const {
        auto a = vehicle.MaxAcceleration;
        auto d = GetRemainingDistance();
        auto v = m_CurrentSpeed;
        auto u = m_ArrivalSpeed;
        auto v2 = v * v;
        auto u2 = u * u;

        auto peakSquared = a * d + (v2 + u2) / 2;
        auto poweredTarget = std::min(vehicle.MaxSpeed, peakSquared.Sqrt());
        auto poweredMs = GetRemainingPoweredTime(vehicle);
        auto dA = std::max(Zero, poweredTarget - v);
        auto dB = std::max(Zero, poweredTarget - u);
        if(a * poweredMs >= (dA + dB) * MsPerSec) {
            return poweredTarget;
        }

        auto b = vehicle.BaseAcceleration;
        if(poweredMs == Zero) {
            auto p2 = b * d + (v2 + u2) / 2;
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
		auto v2 = v * v;
		auto u = m_ArrivalSpeed;
		auto u2 = u * u;

        auto poweredMs = GetRemainingPoweredTime(vehicle);

        auto MakeSpeedChange = [&](Speed delta) -> TravelSegment {
            auto braking = delta < Zero;
            auto dv = braking ? -delta : delta;
			auto powered = poweredMs > Zero;
			auto mag = powered ? vehicle.MaxAcceleration : vehicle.BaseAcceleration;

			auto durationTime = std::min(ToWalkerTime(elapsed), dv * MsPerSec / mag);
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
		auto poweredReduction = std::min(speedToLose, vehicle.MaxAcceleration * poweredMs / MsPerSec);
		auto intermediateSpeed = v - poweredReduction;
		auto i2 = intermediateSpeed * intermediateSpeed;

		auto poweredDistance = (v2 - i2) / (vehicle.MaxAcceleration * 2);
        auto unpoweredDistance = intermediateSpeed > m_ArrivalSpeed
			? (i2 - u2) / (vehicle.BaseAcceleration * 2)
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