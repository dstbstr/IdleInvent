#include "Walker/Travel/TravelLeg.h"

#include "Walker/Travel/Vehicle.h"

#include <Math/Kinematics.h>

#include <algorithm>
#include <cmath>

namespace {
    using namespace Walker;
    struct BrakingEstimate {
        Time Duration{};
        Distance DistanceNeeded{};
    };

    BrakingEstimate EstimateBraking(Acceleration b, Speed v, Speed u, Time t) {
        DR_ASSERT(b > Zero && t > Zero && u >= Zero);
        if(b <= Zero || t <= Zero || u < Zero) return {};
        if (v <= u) return {};

        auto brakingSpeed = b * t / MsPerSec;
        auto denom = u + brakingSpeed;
        DR_ASSERT(denom > Zero);
		if (denom <= Zero) return {};

        auto ratio = Quantity::Ratio(v - u, denom);
        auto logFactor = std::isfinite(ratio)
            ? std::log1p(ratio)
            : Quantity::Log10Ratio(v + brakingSpeed, denom) * std::log(10.0);

        auto brakingTime = t * logFactor;
        auto motion = IntegrateLinearDrag(v, -b, t, brakingTime, MsPerSec);

        return {brakingTime, motion.Distance};
    }

    BrakingEstimate EstimateStop(Speed speed, Speed arrivalSpeed, Acceleration baseBrake, Acceleration poweredBrake, Time responseTime, Time availMs) {
        if (speed <= arrivalSpeed) return {};
        if (availMs <= Zero) {
            return EstimateBraking(baseBrake, speed, arrivalSpeed, responseTime);
        }

        auto poweredEstimate = EstimateBraking(poweredBrake, speed, arrivalSpeed, responseTime);
        if (poweredEstimate.Duration <= availMs) {
            return poweredEstimate; // we can fully power brake
        }
        auto first = IntegrateLinearDrag(speed, -poweredBrake, responseTime, availMs, MsPerSec);
        auto remaining = EstimateBraking(baseBrake, first.EndSpeed, arrivalSpeed, responseTime);

        return {
            .Duration = availMs + remaining.Duration,
            .DistanceNeeded = first.Distance + remaining.DistanceNeeded
        };
    }
}

namespace Walker {
    bool TravelLeg::Advance(OwnedVehicle& vehicle, BaseTime elapsed) { 
        if(m_Traveled >= m_TargetDistance) return true;
        if(elapsed <= ZeroTime) return false;

        while(elapsed > ZeroTime) {
            auto segment = CalculateNextSegment(vehicle, elapsed);
            auto distance = ApplySegment(vehicle, segment);
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

        auto responseTime = m_Rates.GetDragResponseTime(vehicle.Kind);
        auto baseBrake = m_Rates.GetBaseAccel(vehicle.Kind);
        auto poweredBrake = m_Rates.GetPoweredAccel(vehicle.Kind);

        auto braking = EstimateStop(v, u, baseBrake, poweredBrake, responseTime, poweredMs);
        if(v > u && braking.DistanceNeeded >= d) {
            return 2 * d * MsPerSec / (v + u);
        }

        auto EstimateTrip = [&](Time drivingTime) -> BrakingEstimate {
			auto poweredTime = std::min(drivingTime, poweredMs);
			auto first = IntegrateLinearDrag(v, poweredBrake, responseTime, poweredTime, MsPerSec);
			auto second = IntegrateLinearDrag(first.EndSpeed, baseBrake, responseTime, drivingTime - poweredTime, MsPerSec);
			auto stop = EstimateStop(second.EndSpeed, u, baseBrake, poweredBrake, responseTime, poweredMs - poweredTime);

            return {
                .Duration = drivingTime + stop.Duration,
				.DistanceNeeded = first.Distance + second.Distance + stop.DistanceNeeded
            };
        };

        auto low = Zero;
        auto high = MsPerSec;
        // find upper bound
        while(EstimateTrip(high).DistanceNeeded < d) {
            low = high;
            auto nextHigh = high * 2;
			if (nextHigh <= high) break; // overflow
            high = nextHigh;
        }

        // narrow within 100 ms
        while(high - low > Time{10}) {
            auto mid = low + (high - low) / 2;
            if(mid <= low || mid >= high) break;
            if(EstimateTrip(mid).DistanceNeeded < d) {
                low = mid;
            } else {
                high = mid;
            }
        }

        return EstimateTrip(high).Duration;
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

        auto responseTime = m_Rates.GetDragResponseTime(vehicle.Kind);
        auto baseBrake = m_Rates.GetBaseAccel(vehicle.Kind);
        auto poweredBrake = m_Rates.GetPoweredAccel(vehicle.Kind);
        
        auto poweredMs = GetRemainingPoweredTime(vehicle);
        auto braking = EstimateStop(v, u, baseBrake, poweredBrake, responseTime, poweredMs);
        auto d = GetRemainingDistance();
        auto powered = poweredMs > Zero;
        auto mag = powered ? poweredBrake : baseBrake;

        auto duration = ToWalkerTime(elapsed);
        if(powered) {
            duration = std::min(duration, poweredMs);
        }
        // if we need to slow down
        if (v > u && braking.DistanceNeeded >= d) {
			duration = std::min(duration, braking.Duration);
            return {
				.Duration = std::max(OneInstant, ToBaseTime(duration)),
                .Rate = -mag,
                .Powered = powered
            };
        }

        auto RequiredDistance = [&](BaseTime candidate) -> Distance {
			auto candidateMs = ToWalkerTime(candidate);
			auto motion = IntegrateLinearDrag(v, mag, responseTime, candidateMs, MsPerSec);
			auto remainingPower = powered ? poweredMs - candidateMs : Zero;
			auto futureBraking = EstimateStop(motion.EndSpeed, u, baseBrake, poweredBrake, responseTime, remainingPower);

			return motion.Distance + futureBraking.DistanceNeeded;
        };

        // binary search for optimal duratino
        auto low = ZeroTime;
        auto high = ToBaseTime(duration);
        if(RequiredDistance(high) <= d) {
            low = high;
        } else {
            while(high - low > OneInstant) {
				auto mid = low + (high - low) / 2;
				if (RequiredDistance(mid) <= d) {
					low = mid;
				}
				else {
					high = mid;
				}
            }
        }

        return {
            .Duration = std::max(OneInstant, low),
            .Rate = mag,
			.Powered = powered
        };
    }

    Distance TravelLeg::ApplySegment(OwnedVehicle& vehicle, const TravelLeg::TravelSegment& segment) {
		using namespace Walker::Literals;
        auto precision = Quantity{1000}; // calculate in micrometers
        auto motion = IntegrateLinearDrag(
            m_CurrentSpeed * precision, 
            segment.Rate * precision, 
            m_Rates.GetDragResponseTime(vehicle.Kind), 
            ToWalkerTime(segment.Duration), MsPerSec);

        m_CurrentSpeed = motion.EndSpeed / precision;
        m_CurrentAcceleration = segment.Rate - m_Rates.GetDrag(vehicle.Kind, m_CurrentSpeed);

        if(segment.Powered) {
            m_PoweredTime += segment.Duration;

            auto totalFuelOwed = Quantity{ m_PoweredTime.count() } * 1_Kg / vehicle.Efficiency;
            auto fuelBurned = std::clamp(totalFuelOwed - m_FuelConsumed, Zero, vehicle.FuelMass);

            vehicle.FuelMass -= fuelBurned;
            m_FuelConsumed += fuelBurned;
        }

		auto accumulated = motion.Distance + m_DistanceRemainder;
        auto distance = accumulated / precision;
        m_DistanceRemainder = accumulated - distance * precision;
        return distance;
    }
}