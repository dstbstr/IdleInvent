#include "Ui/Screens/Resources.h"

#include "Ui/Ui.h"

#include "InventLib/GameState/GameState.h"

#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

#include <format>

namespace {
    Invent::GameState* gameState{nullptr};
}

namespace Ui::Screens::Resources {
    bool Initialize() {
        gameState = &ServiceLocator::Get().GetRequired<Invent::GameState>();
        return true;
    }
    void Render() { 
        ImGui::Begin("ActiveResources", nullptr, BaseFlags);
        for(auto& [name, resource] : gameState->CurrentResources) {
            ImGui::Checkbox(ToString(name).c_str(), &resource.Progress.Active);
            auto progress = static_cast<f32>(resource.Current) / static_cast<f32>(resource.Max);
            ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), std::format("{}/{}", resource.Current, resource.Max).c_str());
        }
        ImGui::End();

#undef DoResource
    }
}