#include "GhostHunter/Ui/Screens/SettingsScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/GameState/GameSettings.h"

namespace {
    GhostHunter::GameSettings* gameSettings{nullptr};
}
namespace GhostHunter::Ui::Screens::Settings {
    bool Initialize() { 
        auto& services = ServiceLocator::Get();
        gameSettings = &services.GetRequired<GhostHunter::GameSettings>();

        return true; 
    }

    void ShutDown() {}

    void Render() {
        ImGui::Checkbox("Show FPS", &gameSettings->ShowFps);
    }
} // namespace GhostHunter::Ui::Screens::Settings