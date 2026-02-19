#include "GhostHunter/Teams/TeamMember.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "Manage/TickManager.h"
#include "GhostHunter/GameState/Life.h"

namespace GhostHunter {
	TeamMember::TeamMember(MemberName id) : Id(id) {
		auto& services = ServiceLocator::Get();
		services.GetRequired<PubSub<InvestigationStart>>().Subscribe(m_PsHandles, [id](const auto&) {
            auto& members = ServiceLocator::Get().GetRequired<Life>().GetTeam().Members;
            members.at(id).OnStartInvestigation();
        });
		services.GetRequired<PubSub<InvestigationEnd>>().Subscribe(m_PsHandles, [id](const auto&) {
            auto& members = ServiceLocator::Get().GetRequired<Life>().GetTeam().Members;
            members.at(id).OnEndInvestigation();
        });
	}

	void TeamMember::Tick([[maybe_unused]] BaseTime elapsed) {
		if(m_CurrentTool && m_CurrentRoom) {
            m_CurrentTool->Collect(elapsed, m_CurrentRoom);
		}
	}

	void TeamMember::OnStartInvestigation() {
		if(!m_TickHandle) {
			m_TickHandle = ServiceLocator::Get().GetRequired<TickManager>().Register(*this);
		}
    }
	void TeamMember::OnEndInvestigation() {
		m_CurrentTool = nullptr;
		m_CurrentRoom = nullptr;
        m_TickHandle.reset();
    }
}