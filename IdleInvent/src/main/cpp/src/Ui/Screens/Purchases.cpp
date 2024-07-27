#include "Ui/Screens/Purchases.h"

#include "Ui/Ui.h"

#include "InventLib/Mechanics/Unlockable.h"
#include "InventLib/Mechanics/Purchasable.h"
#include "InventLib/Character/Society.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Settings/GameSettings.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

#include <unordered_map>
#include <algorithm>
#include <ranges>

namespace {
    std::unordered_map<std::string, Invent::Purchasable>* Purchasables{};
    std::unordered_map<std::string, Invent::Unlockable>* Unlockables{};
    //Invent::GameState* GameState{nullptr};
    Invent::GameSettings* Settings{nullptr};
    Invent::Society* Society{nullptr};
}

namespace Ui::Screens::Purchases {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        Purchasables = &services.GetRequired<std::unordered_map<std::string, Invent::Purchasable>>();
        Unlockables = &services.GetRequired<std::unordered_map<std::string, Invent::Unlockable>>();
        //GameState = &services.GetRequired<Invent::GameState>();
        Society = &services.GetRequired<Invent::Society>();
        Settings = &services.GetOrCreate<Invent::GameSettings>();

        return true;
    }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("PurchasesScreen", nullptr, BaseFlags);

        std::vector<Invent::Purchasable> purchasables = *Purchasables | 
            std::views::values | 
            std::ranges::to<std::vector<Invent::Purchasable>>();
        std::ranges::sort(purchasables, {}, &Invent::Purchasable::Name);

        auto windowSize = ImGui::GetContentRegionAvail();
        windowSize.y = 300;

        for(auto& purchasable : purchasables) {
            auto disabled = !purchasable.CanPurchase(Society->CurrentLife.Resources);
            if(disabled) {
                ImGui::BeginDisabled();
            }
            ImGui::BeginChild(purchasable.Name.c_str(), windowSize, ImGuiChildFlags_Border);
            ImGui::TextWrapped("%s", purchasable.Name.c_str());
            ImGui::TextWrapped("%s", purchasable.Description.c_str());
            if(Settings->showResourceCosts) {
                ImGui::TextWrapped("Costs: %s", purchasable.CostDescription.c_str());
            }
            if(ImGui::Button("Purchase")) {
                Invent::Purchasables::TryPurchase(purchasable.Name);
            }
            ImGui::EndChild();
            if(disabled) {
                ImGui::EndDisabled();
            }
        }

        ImGui::BeginDisabled();
        for(auto& [name, unlockable] : *Unlockables) {
            ImGui::Text("%s", name.c_str());
        }
        ImGui::EndDisabled();
        ImGui::End();
    }
}