#include "Ui/Components/ProjectComponent.h"
#include "Ui/Screens/Explore.h"
#include "Ui/Ui.h"

#include "InventLib/Character/Society.h"
#include "InventLib/Inventory/Item.h"
#include "InventLib/Projects/Expeditions.h"
#include "InventLib/Projects/Project.h"

#include "Core/Constexpr/ConstexprStrUtils.h"
#include "Core/Constexpr/ConstexprMath.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include <format>

namespace {
    Invent::Society* Society{nullptr};
    Invent::Project* SelectedExpedition{nullptr};
    Invent::GameSettings* Settings{nullptr};

    bool ProjectStarted{false};
    size_t DedicatedWorkers{0};
    std::vector<size_t> ExpeditionItems{};
    std::string LastExpeditionResult{};

    void ApplyItems(Invent::Modifier& mod) {
        for(auto id: ExpeditionItems) {
            auto item = Invent::FindItemById(id).value();
            for(auto effect: item.Effects) {
                mod += effect.Modifier;
            }
        }
    }

    void RenderPrepare() {
        auto& life = Society->CurrentLife;

        Ui::Components::Project::RenderPlusMinus(life.AvailableWorkers, DedicatedWorkers, Settings->PurchaseChoice);

        ImGui::SameLine();
        ImGui::Text("%s", SelectedExpedition->Name.c_str());

        ImGui::SameLine();
        if(ImGui::Button("Invest")) {
            SelectedExpedition->Invest(life.Resources);
        }
        if(ImGui::CollapsingHeader(
               "ResourceCosts", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanTextWidth
           )) {
            for(const auto& resource: Invent::AllResources()) {
                auto remaining = SelectedExpedition->ResourceCost.at(resource).Current -
                                 SelectedExpedition->ResourceProgress.at(resource).Current;
                if(remaining > 0) {
                    auto name = Invent::ToString(resource);
                    ImGui::Text("%s", std::format("{} {}", name, remaining).c_str());
                }
            }
        }
    }

    void RenderShop() {
        ImGui::Text("Currently equipped");
        std::vector<size_t> toRemove{};
        for(auto id: ExpeditionItems) {
            ImGui::PushID(id);
            auto item = Invent::FindItemById(id).value();
            ImGui::Text("%s", item.Name.c_str());
            ImGui::SameLine();
            if(ImGui::SmallButton("x")) {
                toRemove.push_back(id);
            }
            ImGui::PopID();
        }
        for(auto id: toRemove) {
            ExpeditionItems.erase(
                std::remove(ExpeditionItems.begin(), ExpeditionItems.end(), id), ExpeditionItems.end()
            );
        }

        if(ImGui::CollapsingHeader("Shop")) {
            auto allItems = Invent::GetItemsByType(Invent::ItemType::Consumable);
            for(const auto& id: ExpeditionItems) {
                auto item = Invent::FindItemById(id).value();
                ImGui::Text("%s", item.Name.c_str());
            }

            for(auto& item: allItems) {
                ImGui::PushID(item.Id);
                ImGui::Text("%s", item.Name.c_str());
                ImGui::TextWrapped("%s", item.Description.c_str());
                if(ImGui::Button("Buy")) {
                    ExpeditionItems.push_back(item.Id);
                }
                ImGui::PopID();
            }
        }
    }

    void RenderExpeditionOdds() {
        auto expedition = Invent::ExpeditionFromString(SelectedExpedition->Name).value();
        auto mod = Society->CurrentLife.BaseExplorationSuccessModifier;
        ApplyItems(mod);
        auto odds = Invent::ToRelativeOdds(Invent::GetExpeditionOdds(expedition, mod));

        auto GetOdds = [](const f32 odds) { return std::format("{:.2f}%", odds * 100.F); };

        ImGui::Text("Odds");
        ImGui::BeginTable("ExpeditionOdds", 4);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Artifact");
        ImGui::TableNextColumn();
        ImGui::Text("Resources");
        ImGui::TableNextColumn();
        ImGui::Text("Nothing");
        ImGui::TableNextColumn();
        ImGui::Text("Tragedy");
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", GetOdds(odds.Artifact).c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", GetOdds(odds.Success).c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", GetOdds(odds.Nothing).c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", GetOdds(odds.Tragedy).c_str());
        ImGui::EndTable();
    }

    void RenderStartExpedition() {
        if(SelectedExpedition->ResourceProgress.AreAnyLessThan(SelectedExpedition->ResourceCost) || DedicatedWorkers == 0) {
            return;
        }

        if(ImGui::Button("Start")) {
            SelectedExpedition->CurrentWorkers = DedicatedWorkers;
            ApplyItems(Society->CurrentLife.CurrentExplorationSuccessModifier);
            ProjectStarted = true;
        }
    }

    void CancelExpedition() {
        Society->CurrentLife.AvailableWorkers += DedicatedWorkers;
        SelectedExpedition->CurrentWorkers = 0;
        DedicatedWorkers = 0;
        SelectedExpedition->TimeProgress = Invent::BaseTime(0);
        ProjectStarted = false;
    }

    void RenderProgress() {
        auto timeProgress = static_cast<float>(SelectedExpedition->TimeProgress.count()) /
                            static_cast<float>(SelectedExpedition->TimeCost.count());
        auto remainingTime = (SelectedExpedition->TimeCost.count() - SelectedExpedition->TimeProgress.count()) /
                             std::max(size_t(1), SelectedExpedition->CurrentWorkers);
        auto label = std::format("{}", Constexpr::TimeString(remainingTime));
        ImGui::ProgressBar(timeProgress, ImVec2(-1, 0), label.c_str());
        if(ImGui::Button("Cancel")) {
            CancelExpedition();
        }
    }

    void ClearExpedition() {
        SelectedExpedition = nullptr;
        ProjectStarted = false;
        DedicatedWorkers = 0;
        ExpeditionItems.clear();
    }
} // namespace

namespace Ui::Screens::Explore {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        Society = &services.GetRequired<Invent::Society>();
        Settings = &services.GetRequired<Invent::GameSettings>();
        services.GetRequired<PubSub<Invent::ExpeditionOutcome>>().Subscribe(
            [](const Invent::ExpeditionOutcome& outcome) { LastExpeditionResult = ToString(outcome); }
        );
        return true;
    }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("ExploreScreen", nullptr, BaseFlags);
        auto& life = Society->CurrentLife;

        if(!SelectedExpedition) {
            for(auto& project: life.Projects.at(Invent::ProjectType::Explore)) {
                if(ImGui::Button(project.Name.c_str())) {
                    SelectedExpedition = &project;
                }
            }
        } else {
            if(!ProjectStarted) {
                if(ImGui::Button("Back")) {
                    SelectedExpedition = nullptr;
                } else {
                    RenderPrepare();
                    RenderShop();
                    RenderExpeditionOdds();
                    RenderStartExpedition();
                }
            } else {
                if(SelectedExpedition->TimeCost - SelectedExpedition->TimeProgress <= std::chrono::seconds(1)) {
                    ClearExpedition();
                } else {
                    RenderProgress();
                }
            }
        }

        if(!LastExpeditionResult.empty()) {
            ImGui::Text("Last Expedition Result: %s", LastExpeditionResult.c_str());
        }
        ImGui::End();
    }
} // namespace Ui::Screens::Explore