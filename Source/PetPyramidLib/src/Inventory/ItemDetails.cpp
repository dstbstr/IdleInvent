#include "Pets/Inventory/ItemDetails.h"

namespace {
	using namespace Pets;

	constexpr std::array CombatItems{
        CombatItemDetails{CombatItemKind::Distraction, "Distraction", "Distraction", 50},
        CombatItemDetails{CombatItemKind::Net, "Net", "Net", 100},
        CombatItemDetails{CombatItemKind::AtkPotion, "Attack Potion", "AtkPotion", 150},
        CombatItemDetails{CombatItemKind::SpdPotion, "Speed Potion", "SpdPotion", 150},
        CombatItemDetails{CombatItemKind::PiercePotion, "Pierce Potion", "PiercePotion", 150},
        CombatItemDetails{CombatItemKind::Poison, "Poison", "Poison", 200}
    };
	
    constexpr std::array FieldItems{
        FieldItemDetails{FieldItemKind::Bait, "Bait", "Bait", 50},
        FieldItemDetails{FieldItemKind::PetRepelent, "Pet Repelent", "PetRepelent", 50},
        FieldItemDetails{FieldItemKind::Hint, "Hint", "Hint", 100}
    };

    template<typename TKind, typename TDetails, size_t TSize>
    const TDetails& Find(const std::array<TDetails, TSize>& items, TKind kind) {
        auto found = std::ranges::find(items, kind, &TDetails::Kind);
        DR_ASSERT_MSG(found != items.end(), "Item not found");
        return *found;
    }
}

namespace Pets::Details {
    const CombatItemDetails& GetItem(CombatItemKind kind) {
        return Find(CombatItems, kind);
    }

    const FieldItemDetails& GetItem(FieldItemKind kind) {
        return Find(FieldItems, kind);
    }

    std::span<const CombatItemDetails> GetCombatItems() {
        return CombatItems;
    }

    std::span<const FieldItemDetails> GetFieldItems() {
        return FieldItems;
    }
}