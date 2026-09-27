#include "Walker/Home/CrewManager.h"
#include <Utilities/EnumUtils.h>

#include <numeric>

namespace Walker {
	CrewManager::CrewManager() {
		m_State.fill({
			.CrewCount = 0,
			.Completions = 0,
			.Progress = {},
			.Rate = WorkRate{1},
			.WorkRemainder = Zero
		});
		// player is always 'traveling'
		m_State[static_cast<size_t>(CrewRole::Traveling)].CrewCount = 1;
		StartJob(CrewRole::Scout);
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

	Work CrewManager::GetRequiredWork(CrewRole role, u64 completions) const {
		switch(role) {
			using enum CrewRole;
			case Scout: return Work{10} * (completions + 1);
			default: return Work{ 1'000 } * (completions + 1);
		}
	}

	void CrewManager::StartJob(CrewRole role, Work initialWork) {
		auto& state = m_State[static_cast<size_t>(role)];
		if(state.Progress) return;

		state.Progress = JobProgress{
			.RequiredWork = GetRequiredWork(role, state.Completions),
			.CompletedWork = initialWork
		};
	}

	void CrewManager::FinishJob(CrewRole role) {
		auto& state = m_State[static_cast<size_t>(role)];
		if(!state.Progress || !state.Progress->IsComplete()) return;
		auto extraWork = std::max(Zero, state.Progress->CompletedWork - state.Progress->RequiredWork);
		state.Progress.reset();

		state.Completions++;

		if (role == CrewRole::Scout) {
			StartJob(role, extraWork);
		} else {
			TryUnassign(state.CrewCount, role); // roles which don't auto restart get unassigned
			state.WorkRemainder = Zero;
		}

		m_Ps.Publish({
			.Role = role,
			.Completion = state.Completions 
		});

	}
}