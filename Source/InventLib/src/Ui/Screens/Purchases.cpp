#include "InventLib/Ui/Screens/Purchases.h"
#include "InventLib/Ui/Ui.h"
#include "InventLib/Character/Society.h"
#include "InventLib/GameState/GameSettings.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameState.h>
#include <Mechanics/Unlockable.h>
#include <Mechanics/Purchasable.h>
#include <imgui.h>

#include <unordered_map>
#include <algorithm>
#include <ranges>

namespace {
    //std::unordered_map<std::string, Purchasable>* purchaseables{};
    std::unordered_map<std::string, Unlockable>* unlockables{};
    //Invent::GameState* GameState{nullptr};
    Invent::GameSettings* Settings{nullptr};
    Invent::Society* Society{nullptr};
}

namespace Ui::Screens::Purchases {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        //purchaseables = &services.GetRequired<std::unordered_map<std::string, Purchasable>>();
        unlockables = &services.GetRequired<std::unordered_map<std::string, Unlockable>>();
        //GameState = &services.GetRequired<Invent::GameState>();
        Society = &services.GetRequired<Invent::Society>();
        Settings = &services.GetOrCreate<Invent::GameSettings>();

        return true;
    }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("PurchasesScreen", nullptr, BaseFlags);
        auto windowSize = ImGui::GetContentRegionAvail();
        windowSize.y = 300;

        //auto inventions = Purchasables::GetAvailable<InventionName, Invention>();
        //auto& resources = Society->CurrentLife.Resources;
        /*
        std::vector<Purchasable> purchasables = *purchaseables | 
            std::views::values | 
            std::ranges::to<std::vector<Purchasable>>();
        std::ranges::sort(purchasables, {}, &Purchasable::Name);


        for(auto& purchasable : purchasables) {
            auto disabled = !purchasable.CanPurchase(Society->CurrentLife.Resources);
            if(disabled) {
                ImGui::BeginDisabled();
            }
            ImGui::BeginChild(purchasable.Name.c_str(), windowSize, ImGuiChildFlags_Border);
            ImGui::TextWrapped("%s", purchasable.Name.c_str());
            ImGui::TextWrapped("%s", purchasable.Description.c_str());
            if(ImGui::Button("Purchase")) {
                Purchasables::TryPurchase(purchasable.Name);
            }
            ImGui::EndChild();
            if(disabled) {
                ImGui::EndDisabled();
            }
        }
        */
        ImGui::BeginDisabled();
        for(auto& [name, unlockable] : *unlockables) {
            ImGui::Text("%s", name.c_str());
        }
        ImGui::EndDisabled();
        ImGui::End();
    }
}