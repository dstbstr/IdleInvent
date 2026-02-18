#pragma once
#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Locations/Room.h"

namespace GhostHunter {
	enum struct MemberName {
		Unset,

		You,
		Hilda,
		Larry,
		LueAnn,
		Mervin,
		Agnus,

		COUNT
	};

	std::string ToString(MemberName member);

	class TeamMember {
    public:
        using IdType = MemberName;
        MemberName Id;

		TeamMember(MemberName id);

		const Tool* GetCurrentTool() const { return m_CurrentTool; }
        Tool* GetCurrentTool() { return m_CurrentTool; }
        void SetCurrentTool(Tool* tool) { m_CurrentTool = tool; }

		const Room* GetCurrentRoom() const { return m_CurrentRoom; }
        Room* GetCurrentRoom() { return m_CurrentRoom; }
        void SetCurrentRoom(Room* room) { m_CurrentRoom = room; }

		void Tick(BaseTime elapsed);

	private:
        Tool* m_CurrentTool{nullptr};
        Room* m_CurrentRoom{nullptr};
        std::vector<ScopedHandle> m_PsHandles{};
        std::optional<ScopedHandle> m_TickHandle{};

        void OnStartInvestigation();
        void OnEndInvestigation();

		//specialties/strengths/weaknesses
	};
}
