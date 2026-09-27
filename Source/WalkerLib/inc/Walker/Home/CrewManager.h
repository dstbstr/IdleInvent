#pragma once

#include "Walker/Home/JobProgress.h"
#include "Walker/WalkerUnits.h"

#include <DesignPatterns/PubSub.h>
#include <Platform/NumTypes.h>
#include <Utilities/Handle.h>

#include <array>
#include <optional>

namespace Walker {
	enum struct CrewRole: u8 {
		Idle,
		Traveling, // TODO: split into navigator, pilot, other roles?
		Scout,
		Scientist,
		Engineer,

		COUNT
	};

	struct JobCompleted {
		CrewRole Role{};
		u64 Completion{};
	};

	class CrewManager {
		using JobDoneFn = std::function<void(const JobCompleted&)>;
	public:
		CrewManager();

		void Add(u64 count);
		bool TryAssign(u64 count, CrewRole role);
		bool TryUnassign(u64 count, CrewRole role);
		void ClearTravelers();

		u64 GetCount() const;
		u64 GetCount(CrewRole role) const;

		u64 operator[](CrewRole role) const;

		std::optional<Time> GetEta(CrewRole role) const;
		std::optional<f32> GetProgress(CrewRole role) const;

		void Tick(BaseTime elapsed);
		ScopedHandle Subscribe(const JobDoneFn& callback);
		void Subscribe(std::vector<ScopedHandle>& subs, const JobDoneFn& callback);

	private:
		struct State {
			u64 CrewCount{};
			u64 Completions{};
			std::optional<JobProgress> Progress{};
			WorkRate Rate{};
			Quantity WorkRemainder{};
		};
		std::array<State, static_cast<size_t>(CrewRole::COUNT)> m_State{};
		PubSub<JobCompleted> m_Ps{};

		Work GetRequiredWork(CrewRole role, u64 completions) const;
		void StartJob(CrewRole role, Work initialWork = Zero);
		void FinishJob(CrewRole role);
	};
}