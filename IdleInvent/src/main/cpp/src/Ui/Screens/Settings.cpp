#include "Ui/Screens/Settings.h"
#include "Ui/Ui.h"

#include "InventLib/Settings/GameSettings.h"
#include "InventLib/GameState/FileOperation.h"
#include "InventLib/Character/Society.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

namespace {
    Invent::GameSettings* gameSettings{nullptr};
    PubSub<Invent::FileOperation>* fileOperation{nullptr};
    Invent::Society* society{nullptr};
    std::vector<std::string> projectPriorities{};
}

namespace Ui::Screens::Settings {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        gameSettings = &services.GetRequired<Invent::GameSettings>();
        fileOperation = &services.GetRequired<PubSub<Invent::FileOperation>>();
        society = &services.GetRequired<Invent::Society>();

        for(const auto& priority : gameSettings->ProjectPriority) {
            projectPriorities.push_back(Invent::ToString(priority));
        }

        return true;
    }

    void ShutDown() {}
    
    void Render() { 
        ImGui::Begin("Settings", nullptr, BaseFlags);
        ImGui::Checkbox("Show FPS", &gameSettings->ShowFps);
        ImGui::Text("Project Priorities (drag and drop)");

        // TODO: test if this will work with touch instead of mouse
        auto max = static_cast<int>(projectPriorities.size());
        for(auto i = 0; i < max; i++) {
            auto item = projectPriorities[i].c_str();
            ImGui::Selectable(item);
            if(ImGui::IsItemActive() && ! ImGui::IsItemHovered()) {
                auto next = i + (ImGui::GetMouseDragDelta(0).y < 0 ? -1 : 1);
                if(next >= 0 && next < max) {
                    std::swap(projectPriorities[i], projectPriorities[next]);
                    std::swap(gameSettings->ProjectPriority[i], gameSettings->ProjectPriority[next]);

                    society->CurrentLife.ClearWorkers();
                    society->CurrentLife.ShiftWorkers();

                    ImGui::ResetMouseDragDelta();
                }
            }
        }
        ImGui::Button("Mute");
        if(ImGui::Button("Hard Reset")) {
            fileOperation->Publish(Invent::FileOperation::Delete);
        }
        ImGui::Button("Export");
        ImGui::Button("Import");
        if(ImGui::Button("Save")) {
            fileOperation->Publish(Invent::FileOperation::Save);
        }
        ImGui::End();
    }
}