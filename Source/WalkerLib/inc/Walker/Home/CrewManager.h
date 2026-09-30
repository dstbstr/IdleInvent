#pragma once

#include "Walker/Home/JobProgress.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/WalkerUnits.h"
#include "Walker/WalkerRates.h"

#include <DesignPatterns/PubSub.h>
#include <Platform/NumTypes.h>
#include <Utilities/Handle.h>

#include <array>
#include <optional>
#include <string>

namespace Walker {
	enum struct CrewRole: u8 {
		Idle,
		Traveling, // TODO: split into navigator, pilot, other roles?
		Scout,
		Scientist,
		Engineer,

		COUNT
	};

	std::string ToString(CrewRole role);

	struct JobCompleted {
		CrewRole Role{};
		size_t CompletedKind{};
	};

	class CrewManager {
		using JobDoneFn = std::function<void(const JobCompleted&)>;
	public:
		CrewManager(const WalkerRates& rates);

		void Add(u64 count);
		void Hire(u64 count);
		bool TryAssign(u64 count, CrewRole role);
		bool TryUnassign(u64 count, CrewRole role);
		void ClearTravelers();

		u64 GetCount() const;
		u64 GetCount(CrewRole role) const;
		u64 GetHiredCount() const;

		u64 operator[](CrewRole role) const;

		std::optional<Time> GetEta(CrewRole role) const;
		std::optional<f32> GetProgress(CrewRole role) const;

		void SetNextSearch(EndpointKind kind);

		void Tick(BaseTime elapsed);
		ScopedHandle Subscribe(const JobDoneFn& callback);
		void Subscribe(std::vector<ScopedHandle>& subs, const JobDoneFn& callback);
		void Rebirth();

	private:
		struct State {
			u64 CrewCount{};
			std::optional<JobProgress> Progress{};
			Quantity WorkRemainder{};
		};
		const WalkerRates& m_Rates;
		std::array<State, static_cast<size_t>(CrewRole::COUNT)> m_State{};
		std::array<u64, static_cast<size_t>(EndpointKind::COUNT)> m_ScoutCompletions{};
		PubSub<JobCompleted> m_Ps{};
		EndpointKind m_NextSearch{EndpointKind::Neighborhood};
		EndpointKind m_CurrentSearch{EndpointKind::Neighborhood};
		u64 m_HiredCount{};

		void StartScout(Work initialWork = Zero);
		void FinishJob(CrewRole role);
	};
}