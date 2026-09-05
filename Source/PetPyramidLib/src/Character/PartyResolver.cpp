#include "Pets/Character/PartyResolver.h"
#include "Pets/Pets/PetDetails.h"

#include <cmath>
#include <queue>
#include <vector>
#include <ranges>

namespace {
	using namespace Pets;
	using EntryIndex = size_t;
	inline constexpr EntryIndex InvalidEntry = static_cast<EntryIndex>(-1);

    struct ResolutionEntry {
        const PartyNode* Current{nullptr};
        EntryIndex ParentIndex{InvalidEntry};
		EntryIndex FirstChildIndex{InvalidEntry};
		size_t ChildCount{};
        u32 Depth{};
		const OwnedPet* Owned{};

		PetDetails Details{};
    };

	void ApplyDownward(EntryIndex index, std::vector<ResolutionEntry>& entries, f32& actionRate) {
		auto& entry = entries[index];
		if(entry.ChildCount == 0) return;

		for(auto i = entry.FirstChildIndex; i < entry.FirstChildIndex + entry.ChildCount; ++i) {
			auto& child = entries[i];
			for(const auto& ability : entry.Details.Abilities) {
				if(ability.Level > entry.Owned->Level) break;
				if(ability.Value.Target != AbilityTarget::Pets) continue;
				ApplyAbility(ability.Value, child.Details, actionRate);
            }
		}
	}

	void ApplyUpward(EntryIndex index, std::vector<ResolutionEntry>& entries, f32& actionRate) {
		auto& entry = entries[index];
        if(entry.ParentIndex == InvalidEntry) return;

		auto& parent = entries[entry.ParentIndex];
        for(const auto& ability: entry.Details.Abilities) {
            if(ability.Level > entry.Owned->Level) break;

			switch(ability.Value.Target) { 
				using enum AbilityTarget;
				case Owner: ApplyAbility(ability.Value, parent.Details, actionRate); break;
				case Hero: ApplyAbility(ability.Value, entries[0].Details, actionRate); break;
                case Self: ApplyAbility(ability.Value, entry.Details, actionRate); break;
                case Siblings: {
					for(auto& node : entries) {
						if(node.ParentIndex == entry.ParentIndex && node.Current != entry.Current) {
							ApplyAbility(ability.Value, node.Details, actionRate);
						}
					}
				}
				break;
                default: case Pets: break;
			}
        }
	}

	std::vector<ResolutionEntry> FlattenTree(const Party& party, const PetRoster& roster) {
		std::vector<ResolutionEntry> entries;
		std::queue<EntryIndex> pending;
		auto& hero = roster[PetKind::Hero];
        DR_ASSERT_MSG(hero.has_value(), "Roster must contain Hero");
        if(!hero.has_value()) return {};

		entries.push_back({
			.Current = &party.Hero,
			.ParentIndex = InvalidEntry,
            .FirstChildIndex = InvalidEntry,
			.ChildCount = 0,
			.Depth = 0,
			.Owned = &*hero,
			.Details = Pets::Details::Get(PetKind::Hero)
		});
		pending.push(0);

        while(!pending.empty()) {
			auto index = pending.front();
			pending.pop();

			auto* parentNode = entries[index].Current;
			auto parentDepth = entries[index].Depth;
			auto childStart = entries.size();
			auto childCount = parentNode->Pets.size();

			for(const auto& child : parentNode->Pets) {
                auto& owned = roster[child.Kind];
                DR_ASSERT_MSG(owned.has_value(), "Party contains pet not in roster");
				if(!owned) throw "Wat?";

				auto childIndex = entries.size();

				entries.push_back({
					.Current = &child, 
					.ParentIndex = index, 
					.Depth = parentDepth + 1,
                    .Owned = &*owned,
                    .Details = Pets::Details::Get(child.Kind)
				});

				pending.push(childIndex);
			}

			entries[index].FirstChildIndex = childCount == 0 ? InvalidEntry : childStart;
			entries[index].ChildCount = childCount;
		}

		return entries;
	}
}

namespace Pets::PartyResolver {
	PartyResolution Resolve(const Party& party, const PetRoster& roster) {
		f32 actionRate = 0.5;

		auto entries = FlattenTree(party, roster);

		for(EntryIndex i = 0; i < entries.size(); ++i) {
            ApplyDownward(i, entries, actionRate);
        }
		for(EntryIndex i = entries.size(); i-- > 0;) {
			ApplyUpward(i, entries, actionRate);
		}

		
		std::vector<ResolvedPet> pets = std::views::transform(entries, [](const ResolutionEntry& entry) {
            return ResolvedPet{
				.Node = entry.Current,
				.ParentIndex = entry.ParentIndex,
				.Depth = entry.Depth,
                .Kind = entry.Current->Kind,
                .Attack = entry.Details.Attack,
                .Piercing = entry.Details.Piercing,
                .PetCapacity = entry.Details.PetCapacity
            };
        }) | std::ranges::to<std::vector>();

		return PartyResolution {
            .Pets = pets,
			.ActionRate = actionRate
		};
	}
}