#include "Walker/Milestones/Milestones.h"
#include "Walker/Milestones/WalkerStats.h"
#include "Walker/Journey/Journey.h"

#include <Instrumentation/Logging.h>
#include <Utilities/EnumUtils.h>

#include <algorithm>
#include <array>
#include <format>
#include <ranges>
#include <utility>

namespace {
	using namespace Walker;
	using namespace Walker::Literals;

	constexpr std::array TravelThresholds{100_Km, 10_Mm, 1_Ly, 100_Ly, 10'000_Ly};
	constexpr std::array WalkingThresholds{ 1_Km, 10_Km, 100_Km, 1_Mm, 10_Mm };

	constexpr std::array StatDefinitions{
		MilestoneDetails{MilestoneKind::Travel, TravelThresholds, [](const WalkerStats& stats) -> Quantity {
			return stats.TotalDistance;
		}},
		MilestoneDetails{MilestoneKind::Walking, WalkingThresholds, [](const WalkerStats& stats) -> Quantity {
			return stats.DistanceByVehicle.at(static_cast<size_t>(VehicleKind::Foot));
		}}
	};

	const std::array OneTimeDefinitions{
		OneTimeMilestoneDetails{MilestoneKind::JetToNeighbor, MilestoneTriggerEvent::EndpointReached, [](const HomeBase& home, const Journey* journey) -> bool {
			return journey 
				&& journey->GetEndpoint() 
				&& journey->GetVehicleKind() == VehicleKind::Jet 
				&& journey->GetEndpoint()->Kind == EndpointKind::Neighborhood;
		}}
	};

	const auto Presentations = [] {
		std::array<MilestonePresentation, static_cast<size_t>(MilestoneKind::COUNT)> result{};
		auto Normal = [&](MilestoneKind kind, std::string name, std::string benefit) {
			result.at(static_cast<size_t>(kind)) = {
				std::move(name),
				std::move(benefit),
				false
			};
		};

		auto Secret = [&](MilestoneKind kind, std::string name, std::string benefit) {
			result.at(static_cast<size_t>(kind)) = { 
				std::move(name), 
				std::move(benefit), 
				true 
			};
		};

		using enum MilestoneKind;
		Normal(Travel, "Traveler", "");
		Normal(Walking, "Walker", "");
		Secret(JetToNeighbor, "Overkill", "");
		
		return result;
	}();
}

namespace Walker {
	const MilestonePresentation& GetMilestonePresentation(MilestoneKind kind) {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid milestone kind");
		return Presentations.at(static_cast<size_t>(kind));
	}

	const MilestoneDetails* TryGetMilestoneDetails(MilestoneKind kind) {
		auto it = std::ranges::find(StatDefinitions, kind, &MilestoneDetails::Kind);
		return it == StatDefinitions.end() ? nullptr : &(*it);
	}
	
	const OneTimeMilestoneDetails* TryGetOneTimeMilestoneDetails(MilestoneKind kind) {
		auto it = std::ranges::find(OneTimeDefinitions, kind, &OneTimeMilestoneDetails::Kind);
		return it == OneTimeDefinitions.end() ? nullptr : &(*it);
	}

	std::span<const MilestoneDetails> GetTieredMilestones() {
		return StatDefinitions;
	}

	std::span<const OneTimeMilestoneDetails> GetOneTimeMilestones() {
		return OneTimeDefinitions;
	}

	std::string DescribeMilestone(MilestoneKind kind, size_t tier) {
		DR_ASSERT_MSG(Enum::IsValid(kind), "Invalid milestone kind");
		DR_ASSERT_MSG(tier > 0, "Tier must be greater than 0");

		using enum MilestoneKind;
		if(const auto* tierDetails = TryGetMilestoneDetails(kind)) {
			DR_ASSERT_MSG(tier <= tierDetails->Thresholds.size(), "Tier exceeds available thresholds");
			auto target = tierDetails->Thresholds[tier - 1];
			auto targetStr = target.ToHumanReadable(2, 3).value_or(target.ToScientific(2, 3));
			switch(kind) {
			case Travel: return std::format("Travel {}m or more", targetStr);
			case Walking: return std::format("Walk {}m or more", targetStr);
			}
		}
		else if (const auto* oneTimeDetails = TryGetOneTimeMilestoneDetails(kind)) {
			DR_ASSERT_MSG(tier == 1, "One-time milestones only have a single tier");
			switch (kind) {
			case JetToNeighbor: return "Take a jet somewhere in the neighborhood";
			default: DR_ASSERT_MSG(false, "Unknown one-time milestone kind");
			}
		}
		else {
			DR_ASSERT_MSG(false, "Unknown milestone kind");
		}
		return "";
	}
}