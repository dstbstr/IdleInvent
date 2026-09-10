#include "Pets/Ui/Screens/StoreScreen.h"

#include "Pets/Inventory/Inventory.h"
#include "Pets/Inventory/Items.h"
#include "Pets/Inventory/ItemDetails.h"

#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

namespace {
    constexpr std::array<std::pair<Pets::CombatItemKind, const char*>, 6> CombatItems{{
        {Pets::CombatItemKind::Distraction, "Distraction"},
        {Pets::CombatItemKind::Net, "Net"},
        {Pets::CombatItemKind::AtkPotion, "AtkPotion"},
        {Pets::CombatItemKind::SpdPotion, "SpdPotion"},
        {Pets::CombatItemKind::PiercePotion, "PiercePotion"},
        {Pets::CombatItemKind::Poison, "Poison"}
    }};

    constexpr std::array<std::pair<Pets::FieldItemKind, const char*>, 3> FieldItems{{
         {Pets::FieldItemKind::Bait, "Bait"},
         {Pets::FieldItemKind::PetRepelent, "PetRepelent"},
         {Pets::FieldItemKind::Hint, "Hint"}
    }};

    Pets::Inventory* Inv{nullptr};
}

namespace Pets::Ui::Screens::Store {
    bool Initialize() { 
        bool valid = true;
        for(const auto& [item, name] : CombatItems) {
            valid &= Graphics::IsSpriteValid(name);
            DR_ASSERT(valid);
        }
        for(const auto& [item, name]: FieldItems) {
            valid &= Graphics::IsSpriteValid(name);
            DR_ASSERT(valid);
        }

        auto& services = ServiceLocator::Get();
        Inv = &services.GetRequired<Inventory>();
        return valid;
    }

    void Render() { 
        ImVec2 itemSize = {64.f, 64.f}; // TODO: replace this with adaptive numbers

        for(const auto& [item, name] : CombatItems) {
            auto sprite = Graphics::GetSprite(name);
            auto details = Details::GetItem(item);
            if(SpriteButton(name, sprite, itemSize) && Inv->TrySpendGold(details.Cost)) {
                Inv->Add(item);
            }
        }
        for(const auto& [item, name]: FieldItems) {
            auto sprite = Graphics::GetSprite(name);
            auto details = Details::GetItem(item);
            if(SpriteButton(name, sprite, itemSize) && Inv->TrySpendGold(details.Cost)) {
                Inv->Add(item);
            }
        }
    }
    void ShutDown() {}
} // namespace Pets::Ui::Screens::Store