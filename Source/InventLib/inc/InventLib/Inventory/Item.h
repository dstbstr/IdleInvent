#pragma once

#include "GameState/GameTime.h"
#include "Mechanics/Effect.h"

#include <string>
#include <optional>
#include <vector>

namespace Invent {
    enum struct ItemType {
        Consumable,
        Quest,

        // Equipment
        Head,
        Torso,
        Belt,
        Legs,
        Feet,
        PrimaryHand,
        OffHand,

        // Accessories
        Ring,
        Gloves,
        Neck,
        Wrist,
        Earring,
        Goggles,
        Tattoo,

        EQUIPMENT_BEGIN = Head,
        EQUIPMENT_END = OffHand,
        ACCESSORY_BEGIN = Ring,
        ACCESSORY_END = Tattoo
    };

    struct Item {
        size_t Id{0};
        std::string Name{"Unknown"};
        std::string Description{"No description available"};
        ItemType Type{ItemType::Consumable};
        size_t ItemQuality{0};
        std::vector<Effect> Effects{};  
    };

    struct Consumable : public Item {
        size_t MaxStackSize{1};
        size_t CurrentStack{0};
        BaseTime Duration{0};
	};

    std::optional<Item> FindItemById(size_t id);
    std::optional<Item> FindItemByName(const std::string& name);

    std::vector<Item> GetItemsByType(ItemType type);

    constexpr std::string ToString(ItemType type) {
		switch(type) {
			case ItemType::Consumable: return "Consumable";
			case ItemType::Quest: return "Quest";
			case ItemType::Head: return "Head";
			case ItemType::Torso: return "Torso";
			case ItemType::Belt: return "Belt";
			case ItemType::Legs: return "Legs";
			case ItemType::Feet: return "Feet";
			case ItemType::PrimaryHand: return "PrimaryHand";
			case ItemType::OffHand: return "OffHand";
			case ItemType::Ring: return "Ring";
			case ItemType::Gloves: return "Gloves";
			case ItemType::Neck: return "Neck";
			case ItemType::Wrist: return "Wrist";
			case ItemType::Earring: return "Earring";
			case ItemType::Goggles: return "Goggles";
			case ItemType::Tattoo: return "Tattoo";
		}

		return "Unknown";
	}
}