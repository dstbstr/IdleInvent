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

    template<typename TItems>
    void RenderItems(const char* tableId, const TItems& items, int columns) {
        auto& style = ImGui::GetStyle();
        auto availableWidth = ImGui::GetContentRegionAvail().x;
        auto columnWidth = availableWidth / static_cast<f32>(columns);
        auto imageWidth = columnWidth - style.CellPadding.x * 2.f - style.FramePadding.x * 2.f;
        auto imageSize = ImVec2{imageWidth, imageWidth};
        if(ImGui::BeginTable(tableId, columns, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings)) {
            for(const auto& [item, spriteName] : items) {
                ImGui::TableNextColumn();

                auto& details = Pets::Details::GetItem(item);
                auto canAfford = Inv->GetGold() >= details.Cost;
                ImGui::BeginDisabled(!canAfford);
                if(SpriteButton(spriteName, Graphics::GetSprite(spriteName), imageSize)) {
                    if(Inv->TrySpendGold(details.Cost)) {
                        Inv->Add(item);
                    }
                }
                ImGui::EndDisabled();

                ImGui::TextUnformatted(details.Name.data(), details.Name.data() + details.Name.size());
                ImGui::Text("Cost: %llu", details.Cost);
                ImGui::Text("Owned: %zu", Inv->Count(item));
            }

            ImGui::EndTable();
        }
    }
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
        
        ImGui::TextUnformatted("Gold");
        ImGui::SameLine();
        ImGui::Text("%llu", Inv->GetGold());

        ImGui::SeparatorText("Combat Items");
        RenderItems("##CombatStore", CombatItems, 3);

        ImGui::SeparatorText("Field Items");
        RenderItems("##FieldStore", FieldItems, 3);        
    }
    void ShutDown() {
        Inv = nullptr;
    }
} // namespace Pets::Ui::Screens::Store