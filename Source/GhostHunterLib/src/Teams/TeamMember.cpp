#include "GhostHunter/Teams/TeamMember.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "Manage/TickManager.h"

namespace GhostHunter {
	TeamMember::TeamMember(MemberName id) : Id(id) {
		auto& services = ServiceLocator::Get();
		services.GetRequired<PubSub<InvestigationStart>>().Subscribe(m_PsHandles, [this](const auto&) {
			OnStartInvestigation();
        });
		services.GetRequired<PubSub<InvestigationEnd>>().Subscribe(m_PsHandles, [this](const auto&) {
			OnEndInvestigation();
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