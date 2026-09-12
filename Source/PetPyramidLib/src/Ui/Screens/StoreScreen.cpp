#include "Pets/Ui/Screens/StoreScreen.h"

#include "Pets/Inventory/Inventory.h"
#include "Pets/Inventory/Items.h"
#include "Pets/Inventory/ItemDetails.h"

#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

namespace {
    Pets::Inventory* Inv{nullptr};
    ButtonColors StoreButtonColors{
        .BackgroundColor = IM_COL32(0, 0, 0, 255),
        .HoveredColor = IM_COL32(20, 20, 20, 255),
        .ActiveColor = IM_COL32(50, 50, 50, 255)
    };

    template<typename TItems>
    void RenderItems(const char* tableId, const TItems& items, int columns) {
        auto& style = ImGui::GetStyle();
        auto availableWidth = ImGui::GetContentRegionAvail().x;
        auto columnWidth = availableWidth / static_cast<f32>(columns);
        auto imageWidth = columnWidth - style.CellPadding.x * 2.f - style.FramePadding.x * 2.f;
        auto imageSize = ImVec2{imageWidth, imageWidth};
        if(ImGui::BeginTable(tableId, columns, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings)) {
            for(const auto& details : items) {
                ImGui::TableNextColumn();

                auto canAfford = Inv->GetGold() >= details.Cost;
                auto spriteName = std::string(details.SpriteName);
                ImGui::BeginDisabled(!canAfford);
                if(SpriteButton(spriteName.c_str(), Graphics::GetSprite(spriteName), imageSize, StoreButtonColors)) {
                    if(Inv->TrySpendGold(details.Cost)) {
                        Inv->Add(details.Kind);
                    }
                }
                ImGui::EndDisabled();

                ImGui::TextUnformatted(details.Name.data(), details.Name.data() + details.Name.size());
                ImGui::Text("Cost: %llu", details.Cost);
                ImGui::Text("Owned: %zu", Inv->Count(details.Kind));
            }

            ImGui::EndTable();
        }
    }
}

namespace Pets::Ui::Screens::Store {
    bool Initialize() { 
        bool valid = true;
        for(const auto& details : Details::GetCombatItems()) {
            valid &= Graphics::IsSpriteValid(std::string(details.SpriteName));
            DR_ASSERT(valid);
        }
        for(const auto& details: Details::GetFieldItems()) {
            valid &= Graphics::IsSpriteValid(std::string(details.SpriteName));
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
        RenderItems("##CombatStore", Details::GetCombatItems(), 3);

        ImGui::SeparatorText("Field Items");
        RenderItems("##FieldStore", Details::GetFieldItems(), 3);
    }
    void ShutDown() {
        Inv = nullptr;
    }
} // namespace Pets::Ui::Screens::Store