#pragma once

#include <Platform/NumTypes.h>

#include <array>

namespace Walker {
	enum struct CrewRole: u8 {
		Idle,
		Traveling, // TODO: split into navigator, pilot, other roles?
		Scout,
		Scientist,
		Engineer,

		COUNT
	};

	class CrewRoster {
	public:
		CrewRoster();

		void Add(u64 count);
		bool TryAssign(u64 count, CrewRole role);
		bool TryUnassign(u64 count, CrewRole role);
		void ClearTravelers();

		u64 GetCount() const;
		u64 GetCount(CrewRole role) const;

		u64 operator[](CrewRole role) const;

	private:
		std::array<u64, static_cast<size_t>(CrewRole::COUNT)> m_Roster{};
	};
}