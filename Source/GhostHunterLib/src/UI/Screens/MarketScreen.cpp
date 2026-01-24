#include "GhostHunter/Ui/Screens/MarketScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"

#include "DesignPatterns/ServiceLocator.h"
#include "Mechanics/Purchasable.h"
#include "Mechanics/Sale.h"
#include "Mechanics/Upgrade.h"

#include <imgui.h>

namespace {
    GhostHunter::Life* life{nullptr};

    void RenderSell() {
        auto& allMedia = life->GetInventory().CreatedMedia;

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
        auto available = Purchasables::GetAvailable<ToolName>();
        auto& resources = life->GetInventory().Resources;

        for(const auto& [id, cost]: available) {
            auto name = ToString(id);
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            ImGui::PushID(static_cast<int>(id));
            auto disabled = cost > resources;
            if(disabled) ImGui::BeginDisabled();
            if(ImGui::Button("Purchase")) {
                Purchasables::TryPurchase<ToolName>(id, resources, BuyOnce::Yes);
            }
            if(disabled) ImGui::EndDisabled();
            ImGui::PopID();
            if(disabled) break; // show up to 1 unavailable
        }

        if(available.size() > 0 && owned.size() > 0) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        int id = 0;
        for(auto& tool: owned) {
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