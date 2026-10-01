#include "Walker/Ui/WalkerHeader.h"
#include "Walker/Ui/Screens/WalkerScreens.h"
#include "Walker/WalkerSettings.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>
#include <Ui/Widgets/QuantitySelector.h>
#include <imgui.h>

namespace {
    ImFont* TopFont{nullptr};
    Walker::WalkerSettings* Settings{nullptr};

    void RenderFps() {
        const auto& frameRate = ImGui::GetIO().Framerate;
        TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
    }
} // namespace

namespace Walker::WalkerUi::Header {
    constexpr auto SettingsIcon = "Icons/Settings.png";
    bool Initialize() {
        auto& services = ServiceLocator::Get();
		Settings = &services.GetRequired<WalkerSettings>();
        TopFont = GetFont(FontSizes::H3);
        return TopFont && Graphics::TryLoadImageFile(SettingsIcon);
    }

    f32 GetRequestedHeight(f32) {
        auto& style = ImGui::GetStyle();
        ImGui::PushFont(TopFont);
        auto result = ImGui::GetFontSize() + style.FramePadding.y * 2.f + style.WindowPadding.y * 2.f;
        ImGui::PopFont();
        return result;
    }

    void Render() {
        auto contentOrigin = ImGui::GetCursorPos();

        ImGui::PushFont(TopFont);
        auto iconSize = ImGui::GetFontSize();
        if(ImGui::ImageButton("Settings", Graphics::GetImageHandle(SettingsIcon), {iconSize, iconSize})) {
            if(Screens::GetActiveScreen() == Screen::Settings) {
                Screens::SetActiveScreen(Screen::Travel);
            } else {
                Screens::SetActiveScreen(Screen::Settings);
            }
        }

        ImGui::SetCursorPos(contentOrigin);
        RenderFps();
        ImGui::SameLine(0.f, ImGui::GetContentRegionAvail().x * 0.2f);
        Ui::QuantitySelector("QuantitySelector", Settings->PurchaseSetting);
        ImGui::PopFont();
    }

    void ShutDown() { 
        TopFont = nullptr; 
        Settings = nullptr;
    }
}
