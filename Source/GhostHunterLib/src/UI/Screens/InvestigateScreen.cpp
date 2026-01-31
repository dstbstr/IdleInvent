#include "GhostHunter/Ui/Screens/InvestigateScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/Tools/UseTool.h"

#include "Utilities/EnumUtils.h"
#include "Mechanics/Purchasable.h"

namespace {
    GhostHunter::Life* life{nullptr};
    //Handle currentTool{InvalidHandle};
    size_t currentTool = 1'000;

    GhostHunter::LocationName bestLocation{GhostHunter::LocationName::Unset};
    GhostHunter::LocationName selectedLocation{GhostHunter::LocationName::Unset};

    void RenderSelectLocation() {
        using namespace GhostHunter;
        auto purchaseables = Purchasables::GetAvailable<LocationName>();
        auto& resources = life->GetInventory().Resources;
        for(const auto& [location, cost]: purchaseables) {
            auto text = std::format("{}", ToString(location));
            // TODO: Base on unlocked locations instead of resources
            auto afford = Purchasables::CanPurchase(location, resources);
            if(afford) {
                bestLocation = std::max(bestLocation, location);
            }
            if(ImGui::Button(text.c_str())) {
                selectedLocation = location;
            }
            if(!afford && location >= bestLocation) break;
        }
    }
    void RenderConfirmLocation() {
        using namespace GhostHunter;
        auto& resources = life->GetInventory().Resources;
        auto afford = Purchasables::CanPurchase(selectedLocation, resources);

        ImGui::Text("%s", ToString(selectedLocation).c_str());
        ImGui::TextWrapped("%s", Describe(selectedLocation).c_str());

        ImGui::BeginTable("NavButtons", 2);
        ImGui::TableNextColumn();
        if(ImGui::Button("Back")) {
            selectedLocation = LocationName::Unset;
        }
        
        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!afford);
        if(ImGui::Button("Rent")) {
            Purchasables::TryPurchase(selectedLocation, resources, BuyOnce::No);
            selectedLocation = LocationName::Unset;
        }
        ImGui::EndDisabled();
        ImGui::EndTable();
    }

    void RenderRentLocation() {
        if(selectedLocation == GhostHunter::LocationName::Unset) {
            RenderSelectLocation();
        } else {
            RenderConfirmLocation();
        }
    }

    void RenderUseLocation() {
        using namespace GhostHunter;
        const auto* investigation = life->GetCurrentInvestigation();
        ImGui::Text("%s", investigation->Describe().c_str());
        ImGui::ProgressBar(
            1.0f - investigation->GetProgress(),
            ImVec2(-1, 0), 
            std::format("{}", investigation->Ttl).c_str()
        );

        auto& tools = life->GetInventory().OwnedTools;
        if(currentTool > tools.size()) {
            ImGui::BeginTable("Tools", 2);
            for(size_t i = 0; i < tools.size(); i++) {
                const auto& tool = tools[i];
                ImGui::TableNextColumn();
                ImGui::Text("%s", ToString(tool.Id).c_str());

                ImGui::TableNextColumn();
                ImGui::PushID(static_cast<int>(tool.Id));
                if(ImGui::Button("Use")) {
                    tools[i].Start();
                    currentTool = i;
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        } else {
            ImGui::Text("%s", tools[currentTool].Describe().c_str());
            ImGui::SameLine();
            if(ImGui::Button("Stop")) {
                tools[currentTool].Stop();
                currentTool = 1'000;
            }
        }
    }
}

namespace GhostHunter::Ui::Screens::Investigate {
    bool Initialize() { 
        life = &ServiceLocator::Get().GetRequired<Life>();
        return true; 
    }

    void ShutDown() {}

    void Render() {
        if(life->GetCurrentInvestigation()) {
            RenderUseLocation();
        } else {
            RenderRentLocation();
            currentTool = 1'000;
        }
    }
} // namespace GhostHunter::Ui::Screens::Investigate