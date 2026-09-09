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
        ImGui::PushFont(GetFont(FontSizes::H3));
        TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
        ImGui::PopFont();
    }

    f32 GetIconSize() {
        ImGui::PushFont(GetFont(FontSizes::H3));
        auto size = ImGui::GetFontSize();
        ImGui::PopFont();
        return size;
    }
}

namespace Pets::Ui::Screens::TopContent {
    constexpr auto SettingsIcon = "Icons/Settings.png";
    bool Initialize() {
        auto& services = ServiceLocator::Get();

        return Graphics::TryLoadImageFile(SettingsIcon);
    }

    f32 GetRequestedHeight(f32) {
        auto& style = ImGui::GetStyle();
        return GetIconSize() + style.FramePadding.y * 2.f + style.WindowPadding.y * 2.f;
    }

    void Render() {
        auto iconSize = GetIconSize();
        if(ImGui::ImageButton("Settings", Graphics::GetImageHandle(SettingsIcon), {iconSize, iconSize})) {
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
