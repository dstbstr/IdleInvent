#include "GhostHunter/Ui/Screens/MarketScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"

#include "Mechanics/Purchasable.h"
#include "Mechanics/Sale.h"
#include "Mechanics/Upgrade.h"

namespace {
    GhostHunter::Life* life{nullptr};
    GhostHunter::ToolName bestTool{GhostHunter::ToolName::Unset};

    void RenderSell() {
        auto& allMedia = life->GetInventory().CreatedMedia;
        if(allMedia.empty()) return;
        ImGui::Spacing();
        ImGui::Separator();

        for(size_t i = 0; i < allMedia.size(); i++) {
            const auto& media = allMedia[i];
            ImGui::Text("%s", std::format("{}", media).c_str());
            ImGui::SameLine();
            ImGui::PushID(static_cast<int>(i));
            if(ImGui::Button("Sell")) {
                Sell(allMedia, i);
            }
            ImGui::PopID();
        }

        if(ImGui::Button("Sell All")) {
            SellAll(allMedia);
        }
    }

    void RenderBuy() {
        using namespace GhostHunter;
        auto& owned = life->GetInventory().OwnedTools;
        auto available = life->GetUnlockedTools();
        auto& resources = life->GetInventory().Resources;

        for(auto tool: available) {
            if(owned.contains(tool)) continue;

            ImGui::Text("%s", ToString(tool).c_str());
            ImGui::SameLine();
            ImGui::PushID(static_cast<int>(tool));
            auto cost = Purchasables::GetCost<ToolName>(tool);
            ImGui::BeginDisabled(cost > resources);
            if(ImGui::Button("Purchase")) {
                Purchasables::TryPurchase<ToolName>(tool, resources, BuyOnce::Yes);
            }
            ImGui::EndDisabled();
            ImGui::PopID();
        }

        if(available.size() > 0 && owned.size() > 0) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        int id = 0;
        for(auto& [toolId, tool]: owned) {
            ImGui::Text("%s", tool.Describe().c_str());
            ImGui::SameLine();
            ImGui::PushID(id++);
            bool disabled = !UpgradeManager::CanUpgrade(tool, resources);
            if(disabled) ImGui::BeginDisabled(); 
            if(ImGui::Button("Upgrade")) {
                UpgradeManager::Upgrade(tool, resources);
            }
            if(disabled) ImGui::EndDisabled();
            ImGui::PopID();
        }
    }
}

namespace GhostHunter::Ui::Screens::Market {
    bool Initialize() { 
        life = &ServiceLocator::Get().GetRequired<Life>();
        return true; 
    }

    void ShutDown() {}

    void Render() {
        RenderBuy();
        RenderSell();
    }
} // namespace GhostHunter::Ui::Screens::Market