#pragma once

#include "Walker/Crew/CrewRole.h"
#include "Walker/Crew/JobProgress.h"
#include "Walker/Crew/WalkerTech.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/WalkerUnits.h"

#include <DesignPatterns/PubSub.h>
#include <Platform/NumTypes.h>
#include <Utilities/Handle.h>

#include <array>
#include <optional>
#include <string>

namespace Walker {
	class WalkerRates;

	struct JobCompleted {
		CrewRole Role{};
		size_t CompletedKind{};
	};

	class CrewManager {
		using JobDoneFn = std::function<void(const JobCompleted&)>;
	public:
		CrewManager(const WalkerRates& rates, TechManager& tech);

		void Add(u64 count);
		void Hire(u64 count);
		bool TryAssign(u64 count, CrewRole role);
		bool TryUnassign(u64 count, CrewRole role);
		bool TryReassign(u64 count, CrewRole from, CrewRole to);
		void ClearTravelers();

		u64 GetCount() const;
		u64 GetCount(CrewRole role) const;
		u64 GetHiredCount() const;

		u64 operator[](CrewRole role) const;

		std::optional<Time> GetEta(CrewRole role) const;
		std::optional<f32> GetProgress(CrewRole role) const;

		bool TryStartScience(TechKind kind);
		bool TryStartEngineering(TechKind kind);

		void Tick(BaseTime elapsed);
		ScopedHandle Subscribe(const JobDoneFn& callback);
		void Subscribe(std::vector<ScopedHandle>& subs, const JobDoneFn& callback);
		void RebirthReset();
		void PrestigeReset();

	private:
		struct State {
			u64 CrewCount{};
			std::optional<JobProgress> Progress{};
			Quantity WorkRemainder{};
			u8 TargetKind{};
		};

		const WalkerRates& m_Rates;
		TechManager& m_Tech;
		std::array<State, static_cast<size_t>(CrewRole::COUNT)> m_State{};
		PubSub<JobCompleted> m_Ps{};
		u64 m_HiredCount{};

		void FinishJob(CrewRole role);
		bool CanAssign(CrewRole role, const State& state) const;
		bool CanUnassign(CrewRole role, const State& state, u64 count) const;
	};
}