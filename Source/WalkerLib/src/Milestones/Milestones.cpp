#include "Walker/Milestones/Milestones.h"
#include "Walker/Milestones/WalkerStats.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"

#include <Instrumentation/Logging.h>
#include <Utilities/EnumUtils.h>

namespace Walker {
	void MilestoneManager::UpdateMilestones(const WalkerStats& stats) {
		for(auto kind : Enum::GetAllValues<MilestoneKind>()) {
			auto* details = TryGetMilestoneDetails(kind);
			if(!details || !details->GetValue) continue;

			auto& level = m_Levels.at(static_cast<size_t>(kind));
			if(level >= details->Thresholds.size()) continue;

			auto value = details->GetValue(stats);
			while(level < details->Thresholds.size() && value >= details->Thresholds[level]) {
				Unlock(kind);
			}
		}
	}

	void MilestoneManager::OnEvent(MilestoneTriggerEvent trigger, const HomeBase& home, const Journey* journey) {
		for (auto kind : Enum::GetAllValues<MilestoneKind>()) {
			// already unlocked
			if (GetUnlockedTier(kind) != 0) continue;

			auto* details = TryGetOneTimeMilestoneDetails(kind);
			if (!details || details->Trigger != trigger || !details->Predicate) continue;
			if (details->Predicate(home, journey)) {
				Unlock(kind);
			}
		}
	}

	size_t MilestoneManager::GetUnlockedTier(MilestoneKind kind) const {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid milestone kind");
		if (!Enum::IsValid(kind)) return 0;

		return m_Levels.at(static_cast<size_t>(kind));
	}

	size_t MilestoneManager::operator[](MilestoneKind kind) const {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid milestone kind");
		if(!Enum::IsValid(kind)) return 0;

		return m_Levels.at(static_cast<size_t>(kind));
	}

	bool MilestoneManager::IsUnlocked(MilestoneKind kind) const {
		return GetUnlockedTier(kind) > 0;
	}

	ScopedHandle MilestoneManager::Subscribe(const std::function<void(const MilestoneUnlocked&)>& callback) {
		return m_Ps.Subscribe(callback);
	}

	void MilestoneManager::Subscribe(std::vector<ScopedHandle>& subs, const std::function<void(const MilestoneUnlocked&)>& callback) {
		subs.push_back(Subscribe(callback));
	}

	void MilestoneManager::Unlock(MilestoneKind kind) {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid milestone kind");
		if (!Enum::IsValid(kind)) return;

		auto& level = m_Levels.at(static_cast<size_t>(kind));
		level++;
		m_Ps.Publish({
			.Kind = kind,
			.Tier = level 
		});
	}
}