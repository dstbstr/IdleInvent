#include "GhostHunter/Ui/Screens/MarketScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"

#include "DesignPatterns/ServiceLocator.h"
#include "Mechanics/Purchasable.h"
#include "Mechanics/Sale.h"

#include <imgui.h>

namespace {
    GhostHunter::Life* life{nullptr};

    void RenderSell() {
        auto& allMedia = life->GetInventory().CreatedMedia;
        ImGui::Text("Inventory Size: %zu", allMedia.size());

        for(size_t i = 0; i < allMedia.size(); i++) {
            const auto& media = allMedia[i];
            ImGui::Text("%s", std::format("{}", media).c_str());
            ImGui::SameLine();
            if(ImGui::Button("Sell")) {
                Sell(allMedia, i);
            }
        }

        if(ImGui::Button("Sell All")) {
            SellAll(allMedia);
        }
    }

    void RenderBuy() {
        using namespace GhostHunter;
        auto& owned = life->GetInventory().OwnedTools;
        auto available = Purchasables::GetAvailable<ToolName>();

        for(const auto& [id, cost]: available) {
            auto name = ToString(id);
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            ImGui::PushID(static_cast<int>(id));
            auto disabled = cost > life->GetInventory().Resources;
            if(disabled) ImGui::BeginDisabled();
            if(ImGui::Button("Purchase")) {
                Purchasables::TryPurchase<ToolName>(id, life->GetInventory().Resources, BuyOnce::Yes);
            }
            if(disabled) ImGui::EndDisabled();
            ImGui::PopID();
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
            if(ImGui::Button("Upgrade")) {
                tool.Quality = Enum::Increment(tool.Quality);
            }
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