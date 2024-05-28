#include "Ui/Screens/Settings.h"
#include "Ui/Ui.h"
#include "Settings/GameSettings.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace {
    GameSettings* gameSettings{nullptr};
}

namespace Ui::Screens::Settings {
    bool Initialize() {
        gameSettings = &ServiceLocator::Get().GetOrCreate<GameSettings>();
        return true;
    }

    void Render() { 
        ImGui::Begin("Settings", nullptr, BaseFlags);
        ImGui::Checkbox("Show FPS", &gameSettings->showFps);
        ImGui::End();
    }
}