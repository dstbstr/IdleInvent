#include "Walker/Journey/Journey.h"

#include <algorithm>

namespace Walker {
	Journey::Journey(OwnedVehicle& vehicle, EndpointKind end) 
		: m_Vehicle(vehicle)
		, m_End(end) {
        const auto& details = GetEndpointDetails(end);
        m_EndpointCargo = details.InitialCargo;
        m_EndpointDistance = details.DistanceFromHome;
        m_CurrentSpeed = vehicle.MaxSpeed;
        m_CurrentAccel = vehicle.MaxAcceleration;
	}

	void Journey::Start() {
		if(m_Phase == Phase::Preparing) {
            m_Phase = Phase::Outbound;
		}
	}

	void Journey::Tick(BaseTime elapsed) { 
		if(m_Phase == Phase::Preparing) return;

		m_PendingTime += elapsed;
		while(m_PendingTime > m_UpdateInterval) {
            m_PendingTime -= m_UpdateInterval;

			if(m_Phase == Phase::Outbound || m_Phase == Phase::Returning) {
				if(m_CurrentSpeed < m_Vehicle.MaxSpeed) {
					m_CurrentSpeed += m_CurrentAccel;
				}
				if(m_Phase == Phase::Outbound) {
                    m_CurrentDistance += m_CurrentSpeed;
                    if(m_CurrentDistance >= m_EndpointDistance) {
                        m_CurrentDistance = m_EndpointDistance;
                        m_CurrentSpeed = 0;
                        m_Phase = Phase::Loading;
                    }
				} else {
                    m_CurrentDistance -= std::min(m_CurrentDistance, m_CurrentSpeed);
					if(m_CurrentDistance == Distance{}) {
                        m_CurrentSpeed = 0;
                        m_Phase = Phase::Unloading;
					}
				}
			} else {
				// tick loading progress
			}
		}
	}

	f32 Journey::GetJourneyRatio() const {
		auto result = static_cast<f32>(Distance::Ratio(m_CurrentDistance, m_EndpointDistance));
        return std::clamp(result, 0.f, 1.f);
	}
}