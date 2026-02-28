#include "InventLib/Inventory/Item.h"
#include "InventLib/Inventory/ItemQuality.h"
#include "InventLib/Effects/Effect.h"

#include <GameState/GameTime.h>
#include <Mechanics/Modifier.h>
#include <Platform/NumTypes.h>

#include <format>
#include <unordered_map>


namespace {
    using namespace Invent;

    std::unordered_map<size_t, Item> ItemsById{};
    std::unordered_map<std::string, size_t> ItemIdByName{};

    void AddItem(const Item& item) {
        ItemsById[item.Id] = item;
        ItemIdByName[item.Name] = item.Id;
    }

    // Note: This implementation means you must add new items at the end to maintain backward compatibility
    Item MakeItem(ItemType type, const std::string& name, const std::string& description, const std::vector<Effect>& effects) {
        return Item{.Id = ItemsById.size(), .Name = name, .Description = description, .Type = type, .Effects = effects};
    }

    Consumable MakeConsumable(Item baseItem, size_t maxStackSize, BaseTime duration) {
        Consumable consumable{.MaxStackSize = maxStackSize, .CurrentStack = 0, .Duration = duration};
        consumable.Id = baseItem.Id;
        consumable.Name = baseItem.Name;
        consumable.Description = baseItem.Description;
        consumable.Type = baseItem.Type;
        consumable.Effects = baseItem.Effects;
        return consumable;
    }

    void Initialize() {
        auto CreateExploreItem = [](const std::string& name, const std::string& description, f32 multiplier) {
			return MakeConsumable(
				MakeItem(ItemType::Consumable, name, description, {Effect{.Target = EffectTarget::ExploreSuccessRate, .Modifier = Modifier{.Add = 0, .Mul = multiplier}}}), 10, OneInstant
			);
		};
        AddItem(CreateExploreItem("Rations", "Keeps the crew healthy longer.", 3.0f));
        AddItem(CreateExploreItem("Sextant", "Helps find true north.", 4.0f));
        AddItem(CreateExploreItem("Spyglass", "See danger before it sees you.", 5.0f));
        AddItem(CreateExploreItem("Map", "Learn from those who did this last time.", 6.0f));
        AddItem(CreateExploreItem("Improved Transportation", "Move faster, safer, and in style.", 7.0f));
        AddItem(CreateExploreItem("Improved Tools", "Get the job done faster and better.", 8.0f));
        AddItem(CreateExploreItem("Improved Communication", "No more cans with string!", 9.0f));
        AddItem(CreateExploreItem("Rent a Hero", "Gaston will happily help you out... for a price.", 10.0f));
        AddItem(CreateExploreItem("An Army", "People are much more cooperative when you have an army.", 11.0f));
        AddItem(CreateExploreItem("Alien Technology", "This is basically cheating at this point.", 25.0f));
    }
} // namespace

namespace Invent {
    std::optional<Item> FindItemById(size_t id) {
        return ItemsById.contains(id) ? std::optional<Item>(ItemsById.at(id)) : std::nullopt;
    }

    std::optional<Item> FindItemByName(const std::string& name) {
        return ItemIdByName.contains(name) ? std::optional<Item>(ItemsById.at(ItemIdByName.at(name))) : std::nullopt;
    }

    std::vector<Item> GetItemsByType(ItemType type) {
        static std::once_flag once;
        std::call_once(once, []() { Initialize(); });

		std::vector<Item> items;
		for (const auto& [id, item] : ItemsById) {
			if (item.Type == type) {
				items.push_back(item);
			}
		}
		return items;
	}
} // namespace Invent