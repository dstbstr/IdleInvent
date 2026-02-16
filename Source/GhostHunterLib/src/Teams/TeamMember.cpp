#include "GhostHunter/Teams/TeamMember.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "Manage/TickManager.h"

namespace GhostHunter {
	TeamMember::TeamMember(MemberName id) : Id(id) {
		auto& services = ServiceLocator::Get();
		m_PsHandles.push_back(services.GetRequired<PubSub<InvestigationStart>>().Subscribe([this](const auto&) {
			OnStartInvestigation();
		}));
		m_PsHandles.push_back(services.GetRequired<PubSub<InvestigationEnd>>().Subscribe([this](const auto&) {
			OnEndInvestigation();
        }));
	}

	TeamMember::~TeamMember() {
		PubSubs::Unregister(m_PsHandles);
		if(m_TickHandle) {
			ServiceLocator::Get().GetRequired<TickManager>().Unregister(m_TickHandle);
		}
    }

	void TeamMember::Tick([[maybe_unused]] BaseTime elapsed) {
		if(m_CurrentTool && m_CurrentRoom) {
            m_CurrentTool->Collect(elapsed, m_CurrentRoom);
		}
	}

	void TeamMember::OnStartInvestigation() {
		if(m_TickHandle == InvalidHandle) {
			m_TickHandle = ServiceLocator::Get().GetRequired<TickManager>().Register(*this);
		}
    }
	void TeamMember::OnEndInvestigation() {
		m_CurrentTool = nullptr;
		m_CurrentRoom = nullptr;
        if(m_TickHandle != InvalidHandle) {
			ServiceLocator::Get().GetRequired<TickManager>().Unregister(m_TickHandle);
		}
    }
}