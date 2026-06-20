#include "Pets/Ui/TopContent.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Ui/Screens/Screens.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

#include <format>

namespace {
    void RenderFps() {
        const auto& frameRate = ImGui::GetIO().Framerate;
        TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
    }
}

namespace Pets::Ui::Screens::TopContent {
    constexpr auto SettingsIcon = "Icons/Settings.png";
    bool Initialize() {
        auto& services = ServiceLocator::Get();

        return Graphics::TryLoadImageFile(SettingsIcon);
    }
    void Render() {
        if(ImGui::ImageButton("Settings", Graphics::GetImageHandle(SettingsIcon), {32, 32})) {
            if(Ui::Screens::GetActiveScreen() == Ui::Screen::Settings) {
                Ui::Screens::SetActiveScreen(Ui::Screen::Pets);
            } else {
                Ui::Screens::SetActiveScreen(Ui::Screen::Settings);
            }
        }
        ImGui::SameLine();
        RenderFps();
    }

	void ShutDown() {}
} // namespace Pets::Ui::Screens::TopContent
