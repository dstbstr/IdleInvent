#include "Pets/Ui/TopContent.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Ui/Screens/Screens.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

#include <format>

namespace {
    ImFont* TopFont{nullptr};

    void RenderFps() {
        const auto& frameRate = ImGui::GetIO().Framerate;
        TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
    }
}

namespace Pets::Ui::Screens::TopContent {
    constexpr auto SettingsIcon = "Icons/Settings.png";
    bool Initialize() {
        TopFont = GetFont(FontSizes::H3);
        return TopFont && Graphics::TryLoadImageFile(SettingsIcon);
    }

    f32 GetRequestedHeight(f32) {
        auto& style = ImGui::GetStyle();
        return TopFont->FontSize + style.FramePadding.y * 2.f + style.WindowPadding.y * 2.f;
    }

    void Render() {
        ImGui::PushFont(TopFont);
        auto iconSize = TopFont->FontSize;
        if(ImGui::ImageButton("Settings", Graphics::GetImageHandle(SettingsIcon), {iconSize, iconSize})) {
            if(Ui::Screens::GetActiveScreen() == Ui::Screen::Settings) {
                Ui::Screens::SetActiveScreen(Ui::Screen::Pets);
            } else {
                Ui::Screens::SetActiveScreen(Ui::Screen::Settings);
            }
        }
        RenderFps();
        ImGui::PopFont();
    }

	void ShutDown() {
        TopFont = nullptr;
    }
} // namespace Pets::Ui::Screens::TopContent
