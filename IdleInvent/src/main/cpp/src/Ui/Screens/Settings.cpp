#include "Ui/Screens/Settings.h"
#include "Ui/Ui.h"

#include "InventLib/Settings/GameSettings.h"
#include "InventLib/GameState/FileOperation.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

namespace {
    Invent::GameSettings* gameSettings{nullptr};
    ServiceLocator* services{nullptr};
}

namespace Ui::Screens::Settings {
    bool Initialize() {
        services = &ServiceLocator::Get();
        gameSettings = &services->GetRequired<Invent::GameSettings>();
        return true;
    }

    void ShutDown() {}
    
    void Render() { 
        ImGui::Begin("Settings", nullptr, BaseFlags);
        ImGui::Checkbox("Show FPS", &gameSettings->showFps);
        ImGui::Checkbox("Show Death Details", &gameSettings->showDeathDetails);
        ImGui::Checkbox("Show Resource Costs", &gameSettings->showResourceCosts);
        ImGui::Button("Mute");
        if(ImGui::Button("Hard Reset")) {
            services->GetRequired<PubSub<Invent::FileOperation>>().Publish(Invent::FileOperation::Delete);
        }
        ImGui::Button("Export");
        ImGui::Button("Import");
        if(ImGui::Button("Save")) {
            services->GetRequired<PubSub<Invent::FileOperation>>().Publish(Invent::FileOperation::Save);
        }
        ImGui::End();
    }
}