#include "Walker/Travel/TravelLeg.h"

#include "Walker/Travel/Vehicle.h"

#include <algorithm>

namespace Walker {
    bool TravelLeg::Advance(OwnedVehicle& vehicle, BaseTime elapsed) { 
        if(m_Traveled >= m_TargetDistance) return true;
        if(elapsed <= ZeroTime) return false;

        auto previousSpeed = m_CurrentSpeed;
        if(NeedsBrakes(vehicle, elapsed)) {
            auto burned = BurnFuel(vehicle, m_CurrentSpeed - m_ArrivalSpeed, elapsed);
            ApplyBrakes(vehicle, burned, elapsed);
        } else {
            auto burned = BurnFuel(vehicle, vehicle.MaxSpeed - m_CurrentSpeed, elapsed);
            ApplyAcceleration(vehicle, burned, elapsed);
        }

        auto stepMs = Time{elapsed.count()};
        auto stepDistance = (previousSpeed + m_CurrentSpeed) * stepMs / (MsPerSec * 2);
        m_Traveled = std::min(m_Traveled + stepDistance, m_TargetDistance);
        if(m_Traveled >= m_TargetDistance) {
            m_CurrentSpeed = Zero;
            m_CurrentAcceleration = Zero;
            return true;
        }
        return false;
    }

    BaseTime TravelLeg::BurnFuel(OwnedVehicle& vehicle, Speed requiredChange, BaseTime elapsed) { 
        using namespace Walker::Literals;
        if(elapsed <= ZeroTime) return ZeroTime;
        if(requiredChange <= Zero || vehicle.MaxAcceleration <= Zero) return ZeroTime;
        if(vehicle.FuelMass <= Zero || vehicle.Efficiency <= Zero) return ZeroTime;

        auto secondsRequired = Quantity::Ratio(requiredChange, vehicle.MaxAcceleration);
        auto stepSeconds = std::chrono::duration<double>(elapsed).count();
        auto usefulSeconds = std::min(secondsRequired, stepSeconds);

        auto usefulTime = std::chrono::duration_cast<BaseTime>(std::chrono::duration<double>{usefulSeconds});

        auto fundedMs = (vehicle.FuelMass + m_FuelConsumed) * vehicle.Efficiency / 1_Kg;
        auto affordableMs = std::clamp(fundedMs - Time{m_PoweredTime.count()}, Zero, Time{elapsed.count()});
        auto affordableTime = OneInstant * static_cast<BaseTime::rep>(Time::Ratio(affordableMs, Time{1}));

        auto poweredTime = std::min(usefulTime, affordableTime);
        m_PoweredTime += poweredTime;

        auto totalFuelOwed = Quantity{m_PoweredTime.count()} * 1_Kg / vehicle.Efficiency;
        auto fuelBurned = std::clamp(totalFuelOwed - m_FuelConsumed, Zero, vehicle.FuelMass);

        vehicle.FuelMass -= fuelBurned;
        m_FuelConsumed += fuelBurned;

        return poweredTime;
    }

    Distance TravelLeg::GetRemainingDistance() const {
        return m_TargetDistance - m_Traveled;
    }

    bool TravelLeg::NeedsBrakes(const OwnedVehicle& vehicle, BaseTime elapsed) const {
        // if we accelerate for another step, will there still be room to brake?
        if(m_CurrentSpeed <= Zero) return false;

        auto brakingAccel = vehicle.BaseAcceleration;
        if(brakingAccel <= Zero) {
            throw std::domain_error("Travel requires positive acceleration");
        }

        auto stepMs = Time{elapsed.count()};
        auto nextSpeed = std::min(m_CurrentSpeed + vehicle.MaxAcceleration * stepMs / MsPerSec, vehicle.MaxSpeed);

        auto nextStepDistance = nextSpeed * stepMs / MsPerSec;
        auto stoppingDistance = std::max(Zero, (nextSpeed * nextSpeed - m_ArrivalSpeed * m_ArrivalSpeed) / (brakingAccel * 2));

        return GetRemainingDistance() <= nextStepDistance + stoppingDistance;
    }

    void TravelLeg::ApplyBrakes(const OwnedVehicle& vehicle, BaseTime poweredTime, BaseTime elapsed) {
        m_CurrentAcceleration = Zero;
        if(m_CurrentSpeed <= m_ArrivalSpeed) return;

        poweredTime = std::clamp(poweredTime, ZeroTime, elapsed);

        auto stepMs = Time{elapsed.count()};
        auto poweredMs = Time{poweredTime.count()};
        auto unpoweredMs = stepMs - poweredMs;

        auto weightedAccel = vehicle.MaxAcceleration * poweredMs + vehicle.BaseAcceleration * unpoweredMs;
        auto previousSpeed = m_CurrentSpeed;
        auto delta = weightedAccel / MsPerSec;

        m_CurrentSpeed = std::max(previousSpeed - delta, m_ArrivalSpeed);
        m_CurrentAcceleration = ((m_CurrentSpeed - previousSpeed) * MsPerSec) / stepMs;
    }

    void TravelLeg::ApplyAcceleration(const OwnedVehicle& vehicle, BaseTime poweredTime, BaseTime elapsed) {
        m_CurrentAcceleration = Zero;
        if(m_CurrentSpeed >= vehicle.MaxSpeed) return;

        auto stepMs = Time{elapsed.count()};
        poweredTime = std::clamp(poweredTime, ZeroTime, elapsed);

        auto poweredMs = Quantity{poweredTime.count()};
        auto unpoweredMs = stepMs - poweredMs;

        auto weightedAccel = vehicle.MaxAcceleration * poweredMs + vehicle.BaseAcceleration * unpoweredMs;
        auto previousSpeed = m_CurrentSpeed;
        auto speedIncrease = weightedAccel / MsPerSec;

        m_CurrentSpeed = std::min(previousSpeed + speedIncrease, vehicle.MaxSpeed);
        m_CurrentAcceleration = ((m_CurrentSpeed - previousSpeed) * MsPerSec) / stepMs;
    }
}