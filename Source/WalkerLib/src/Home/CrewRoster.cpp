#include "Walker/Home/CrewRoster.h"

#include <numeric>

namespace Walker {
	CrewRoster::CrewRoster() {
		m_Roster.fill(0);
		// player is always 'traveling'
		m_Roster[static_cast<size_t>(CrewRole::Traveling)] = 1;
	}

	u64 CrewRoster::operator[](CrewRole role) const {
		return m_Roster[static_cast<size_t>(role)];
	}

	void CrewRoster::Add(u64 count) {
		m_Roster[static_cast<size_t>(CrewRole::Idle)] += count;
	}

	bool CrewRoster::TryAssign(u64 count, CrewRole role) {
		auto& idleCount = m_Roster[static_cast<size_t>(CrewRole::Idle)];
		if(idleCount < count) return false;

		auto& roleCount = m_Roster[static_cast<size_t>(role)];

		idleCount -= count;
		roleCount += count;
		return true;
	}

	void CrewRoster::ClearTravelers() {
		TryUnassign((*this)[CrewRole::Traveling] - 1, CrewRole::Traveling);
	}

	bool CrewRoster::TryUnassign(u64 count, CrewRole role) {
		auto& roleCount = m_Roster[static_cast<size_t>(role)];
		if (roleCount < count) return false;
		if (role == CrewRole::Traveling && roleCount - count < 1) return false; // always leave at least one traveling crew

		auto& idleCount = m_Roster[static_cast<size_t>(CrewRole::Idle)];
		roleCount -= count;
		idleCount += count;
		return true;
	}

	u64 CrewRoster::GetCount() const {
		return std::accumulate(m_Roster.begin(), m_Roster.end(), u64(0), std::plus<u64>());
	}

	u64 CrewRoster::GetCount(CrewRole role) const {
		return m_Roster[static_cast<size_t>(role)];
	}
}