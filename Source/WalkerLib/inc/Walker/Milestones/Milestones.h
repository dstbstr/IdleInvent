#pragma once

#include "Walker/WalkerUnits.h"


#include <DesignPatterns/PubSub.h>
#include <Platform/NumTypes.h>
#include <Utilities/Handle.h>

#include <array>
#include <span>
#include <vector>

namespace Walker {
	struct WalkerStats;
	struct HomeBase;
	class Journey;

	enum struct MilestoneKind : u8 {
		Unset,
		Travel,
		Walking,

		// One-time
		JetToNeighbor,

		COUNT
	};

	enum struct MilestoneTriggerEvent : u8 {
		Unset,
		EndpointReached,
		RoundTripComplete,
		EndpointComplete,

		COUNT
	};

	struct MilestonePresentation {
		std::string Name{};
		std::string BenefitDescription{};
		bool Secret{};
	};

	struct MilestoneUnlocked {
		MilestoneKind Kind{};
		size_t Tier{};
	};

	struct MilestoneDetails {
		MilestoneKind Kind{};
		std::span<const Quantity> Thresholds;
		Quantity (*GetValue)(const WalkerStats&){}; // Quantity GetValue(const WalkerStats&);
	};

	struct OneTimeMilestoneDetails {
		MilestoneKind Kind{};
		MilestoneTriggerEvent Trigger{};
		std::function<bool(const HomeBase&, const Journey*)> Predicate;
	};

	const MilestonePresentation& GetMilestonePresentation(MilestoneKind kind);
	const MilestoneDetails* TryGetMilestoneDetails(MilestoneKind kind);
	const OneTimeMilestoneDetails* TryGetOneTimeMilestoneDetails(MilestoneKind kind);
	std::span<const MilestoneDetails> GetTieredMilestones();
	std::span<const OneTimeMilestoneDetails> GetOneTimeMilestones();
	std::string DescribeMilestone(MilestoneKind kind, size_t tier);

	class MilestoneManager {
	public:
		void UpdateMilestones(const WalkerStats& stats);
		void OnEvent(MilestoneTriggerEvent trigger, const HomeBase& home, const Journey* journey);

		size_t GetUnlockedTier(MilestoneKind kind) const;
		size_t operator[](MilestoneKind kind) const;
		bool IsUnlocked(MilestoneKind kind) const;
		
		ScopedHandle Subscribe(const std::function<void(const MilestoneUnlocked&)>& callback);
		void Subscribe(std::vector<ScopedHandle>& subs, const std::function<void(const MilestoneUnlocked&)>& callback);

	private:
		std::array<size_t, static_cast<size_t>(MilestoneKind::COUNT)> m_Levels{};
		PubSub<MilestoneUnlocked> m_Ps;

		void Unlock(MilestoneKind kind);
	};
}