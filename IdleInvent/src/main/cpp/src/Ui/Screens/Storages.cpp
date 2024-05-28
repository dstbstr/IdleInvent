#include "Ui/Screens/Storages.h"

#include "Ui/Ui.h"

#include "InventLib/GameState/GameState.h"

#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

namespace {
    Invent::GameState* gameState{nullptr};
}

namespace Ui::Screens::Storages {
    bool Initialize() {
        gameState = &ServiceLocator::Get().GetRequired<Invent::GameState>();
        return true;
    }

    void Render() {
        ImGui::Begin("StoragesScreen", nullptr, BaseFlags);
        for(auto& [name, storage] : gameState->Storages) {
            auto progress = static_cast<f32>(storage.Advance.CurrentExp) / static_cast<f32>(storage.Advance.ExpToNextLevel);

            ImGui::PushID(&storage);
            ImGui::Text("%s", ToString(name).c_str());
            ImGui::SameLine();
            ImGui::Text("%s", std::format("Level {}", storage.Advance.CurrentLevel).c_str());

            ImGui::Checkbox("##Active", &storage.Advance.Progress.Active);
            ImGui::SameLine();
            ImGui::ProgressBar(progress, ImVec2(0.f, 0.f));
            ImGui::SameLine();
            bool disabled = !storage.CanLevelUp();
            if(disabled) {
                ImGui::BeginDisabled();
            }
            if(ImGui::Button("Level Up")) {
                storage.LevelUp();
            }

            if(disabled) {
                ImGui::EndDisabled();
            }

            ImGui::PopID();
        }
        ImGui::End();
    }
}