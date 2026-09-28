#include "Walker/Home/CrewManager.h"
#include <Utilities/EnumUtils.h>

#include <numeric>

namespace Walker {
	std::string ToString(CrewRole role) {
		switch (role) {
			using enum CrewRole;
			case Idle: return "Idle";
			case Traveling: return "Traveling";
			case Scout: return "Scout";
			case Scientist: return "Scientist";
			case Engineer: return "Engineer";
		}
		return "Unknown";
	}

	CrewManager::CrewManager() {
		m_State.fill({
			.CrewCount = 0,
			.Progress = {},
			.Rate = WorkRate{1},
			.WorkRemainder = Zero
		});
		// player is always 'traveling'
		m_State[static_cast<size_t>(CrewRole::Traveling)].CrewCount = 1;
		StartScout();
	}

	u64 CrewManager::operator[](CrewRole role) const {
		return m_State[static_cast<size_t>(role)].CrewCount;
	}

	void CrewManager::Add(u64 count) {
		m_State[static_cast<size_t>(CrewRole::Idle)].CrewCount += count;
	}

	bool CrewManager::TryAssign(u64 count, CrewRole role) {
		auto& idleCount = m_State[static_cast<size_t>(CrewRole::Idle)].CrewCount;
		if(idleCount < count) return false;

		auto& state = m_State[static_cast<size_t>(role)];
		if(role != CrewRole::Idle && role != CrewRole::Traveling && !state.Progress) return false;

		idleCount -= count;
		state.CrewCount += count;
		return true;
	}

	void CrewManager::ClearTravelers() {
		TryUnassign((*this)[CrewRole::Traveling] - 1, CrewRole::Traveling);
	}

	bool CrewManager::TryUnassign(u64 count, CrewRole role) {
		auto& roleCount = m_State[static_cast<size_t>(role)].CrewCount;
		if (roleCount < count) return false;
		if (role == CrewRole::Traveling && roleCount - count < 1) return false; // always leave at least one traveling crew

		auto& idleCount = m_State[static_cast<size_t>(CrewRole::Idle)].CrewCount;
		roleCount -= count;
		idleCount += count;
		return true;
	}

	u64 CrewManager::GetCount() const {
		return std::accumulate(m_State.begin(), m_State.end(), u64(0), [](u64 total, const auto& state) -> u64 {
			return total + state.CrewCount;
		});
	}

	u64 CrewManager::GetCount(CrewRole role) const {
		return m_State[static_cast<size_t>(role)].CrewCount;
	}

	std::optional<Time> CrewManager::GetEta(CrewRole role) const {
		auto index = static_cast<size_t>(role);
		const auto& state = m_State[index];
		return state.Progress.and_then([&](const auto& progress) {
			return progress.GetEta(state.Rate * state.CrewCount);
		});
	}

	std::optional<f32> CrewManager::GetProgress(CrewRole role) const {
		const auto& state = m_State[static_cast<size_t>(role)];
		return state.Progress.and_then([&](const auto& progress) {
			return std::optional<f32>{progress.GetProgress()};
		});
	}

	void CrewManager::SetNextSearch(EndpointKind kind) {
		DR_ASSERT_MSG(kind > EndpointKind::Unset && kind < EndpointKind::COUNT, "Invalid endpoint");
		m_NextSearch = kind;
	}

	void CrewManager::Tick(BaseTime elapsed) {
		if(elapsed <= ZeroTime) return;
		auto elapsedMs = ToWalkerTime(elapsed);

		for(auto job : Enum::GetAllValues<CrewRole>()) {
			if(job == CrewRole::Idle || job == CrewRole::Traveling) continue;
			auto& state = m_State[static_cast<size_t>(job)];
			if(state.Progress == std::nullopt || state.CrewCount == 0) continue;

			auto numerator = state.Rate * state.CrewCount * elapsedMs + state.WorkRemainder;
			auto work = numerator / MsPerSec;
			state.WorkRemainder = numerator - work * MsPerSec;

			state.Progress->Advance(work);
			while(state.CrewCount > 0 && state.Progress && state.Progress->IsComplete()) {
				FinishJob(job);
			}
		}
	}

	ScopedHandle CrewManager::Subscribe(const JobDoneFn& callback) {
		return m_Ps.Subscribe(callback);
	}

	void CrewManager::Subscribe(std::vector<ScopedHandle>& subs, const JobDoneFn& callback) {
		subs.push_back(Subscribe(callback));
	}

	void CrewManager::StartScout(Work initialWork) {
		auto& state = m_State[static_cast<size_t>(CrewRole::Scout)];
		if(state.Progress) return;

		m_CurrentSearch = m_NextSearch;
		auto completions = m_ScoutCompletions[static_cast<size_t>(m_CurrentSearch)];
		auto requiredWork = Work{10}.Pow(static_cast<u32>(m_CurrentSearch)) * (completions + 1);
		state.Progress = JobProgress{
			.RequiredWork = requiredWork,
			.CompletedWork = initialWork
		};
	}

	void CrewManager::FinishJob(CrewRole role) {
		auto& state = m_State[static_cast<size_t>(role)];
		if(!state.Progress || !state.Progress->IsComplete()) return;
		auto extraWork = std::max(Zero, state.Progress->CompletedWork - state.Progress->RequiredWork);
		state.Progress.reset();

		if (role == CrewRole::Scout) {
			auto kind = static_cast<size_t>(m_CurrentSearch);
			m_ScoutCompletions[kind]++;
			StartScout(extraWork);
			m_Ps.Publish({
				.Role = role,
				.CompletedKind = kind
			});
		} else {
			TryUnassign(state.CrewCount, role); // roles which don't auto restart get unassigned
			state.WorkRemainder = Zero;
			// m_Ps.Publish({role});
		}

	}
}