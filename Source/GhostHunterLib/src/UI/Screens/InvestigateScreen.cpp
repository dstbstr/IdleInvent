#include "GhostHunter/Ui/Screens/InvestigateScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/Tools/UseTool.h"

#include "Utilities/EnumUtils.h"
#include "Mechanics/Purchasable.h"

#include <imgui.h>
#include <optional>

namespace {
    GhostHunter::Life* life{nullptr};
    Handle currentTool{InvalidHandle};
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
        ImGui::Text("%s", ToString(selectedLocation).c_str());
        ImGui::TextWrapped("%s", Describe(selectedLocation).c_str());

        if(ImGui::Button("Back")) {
            selectedLocation = LocationName::Unset;
        }
        ImGui::SameLine();

        auto& resources = life->GetInventory().Resources;
        auto afford = Purchasables::CanPurchase(selectedLocation, resources);
        ImGui::BeginDisabled(!afford);
        if(ImGui::Button("Rent")) {
            Purchasables::TryPurchase(selectedLocation, resources, BuyOnce::No);
            selectedLocation = LocationName::Unset;
        }
        ImGui::EndDisabled();
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

        auto& events = ServiceLocator::Get().GetRequired<EventManager>();
        if(currentTool == InvalidHandle) {
            auto& inventory = life->GetInventory();
            const auto& tools = inventory.OwnedTools;
            ImGui::BeginTable("Tools", 2);
            for(const auto& tool : tools) {
                ImGui::TableNextColumn();
                ImGui::Text("%s", ToString(tool.Id).c_str());

                ImGui::TableNextColumn();
                ImGui::PushID(static_cast<int>(tool.Id));
                if(ImGui::Button("Use")) {
                    auto OnToolDone = [](const IEvent& event) { 
                        currentTool = InvalidHandle;
                        // consider a start/stop tools
                        // have tool generate income per second
                        if(life->GetCurrentInvestigation()) {
                            const auto& result = static_cast<const UseTool&>(event).Result;
                            auto& evidence = life->GetInventory().Resources.at(result.Type);
                            evidence.Current += static_cast<u8>(result.Quality);
                        }
                    };
                    currentTool = events.StartEvent<UseTool>(OnToolDone, tool);
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        } else {
            const auto* toolEvent = events.GetEvent<UseTool>(currentTool);
            ImGui::Text("%s", toolEvent->Describe().c_str());
            ImGui::ProgressBar(
                toolEvent->GetProgress(), ImVec2(-1, 0), std::format("{}", toolEvent->Ttl).c_str()
            );

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
        }
    }
} // namespace GhostHunter::Ui::Screens::Investigate