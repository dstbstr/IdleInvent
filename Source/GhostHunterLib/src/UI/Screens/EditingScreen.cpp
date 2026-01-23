#include "GhostHunter/Ui/Screens/EditingScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Formatting.h"

#include "DesignPatterns/ServiceLocator.h"
#include "Mechanics/Purchasable.h"
#include "Utilities/EnumUtils.h"
#include <imgui.h>

namespace {
    GhostHunter::Life* life{nullptr};
}
namespace GhostHunter::Ui::Screens::Editing {
    bool Initialize() { 
        life = ServiceLocator::Get().Get<Life>();
        return life != nullptr;
    }

    void ShutDown() {}

    void Render() { 
        auto mediaTypes = Enum::GetAllValues<MediaType>();
        ImGui::BeginTable("Media Costs", 2);
        for(const auto& type: mediaTypes) {
            ImGui::TableNextColumn();
            ImGui::Text("%s", ToString(type).c_str());
            ImGui::TableNextColumn();
            auto cost = Purchasables::GetCost(type);
            ImGui::PushID(static_cast<int>(type));
            auto label = std::format("Create ({})", cost.ToCostString());
            if(ImGui::Button(label.c_str())) {
                Purchasables::TryPurchase(type, life->GetInventory().Resources, BuyOnce::No);
            }
            ImGui::PopID();
        }
        ImGui::EndTable();

        ImGui::BeginTable("Upgrade Costs", 2);
        auto& ownedMedia = life->GetInventory().CreatedMedia;
        int id = 0;
        for(auto& media: ownedMedia) {
            ImGui::TableNextColumn();
            ImGui::Text("%s", std::format("{}", media).c_str());
            ImGui::TableNextColumn();
            auto cost = UpgradeCost(media);
            ImGui::PushID(id++);
            auto label = std::format("Upgrade ({})", cost.ToCostString());
            if(ImGui::Button(label.c_str())) {
                media.Quality = Enum::Increment(media.Quality);
                //Purchasables::TryPurchase(media, life->GetInventory().Resources, BuyOnce::No);
            }
            ImGui::PopID();
        }
        ImGui::EndTable();

    }
} // namespace GhostHunter::Ui::Screens::Editing