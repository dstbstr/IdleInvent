#include "GhostHunter/Ui/TopContent.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/UI/Screens/Screens.h"
#include "GhostHunter/UI/Screens/SettingsScreen.h"
#include "GhostHunter/GameState/GameSettings.h"
#include "Ui/UiUtil.h"

#include <Platform/Graphics.h>
#include <DesignPatterns/ServiceLocator.h>

#include <imgui.h>
#include <format>

namespace {
    GhostHunter::GameSettings* gameSettings{nullptr};

    void RenderFps() {
        if(gameSettings->ShowFps) {
            const auto& frameRate = ImGui::GetIO().Framerate;
            TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
        } else {
            ImGui::Text("");
        }
    }
} // namespace

namespace GhostHunter::Ui::Screens::TopContent {
    constexpr auto SettingsIcon = "Icon/SettingsIcon64.png";
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        gameSettings = &services.GetOrCreate<GhostHunter::GameSettings>();

        return Graphics::TryLoadImageFile(SettingsIcon);
    }
    void Render() {
        if(ImGui::ImageButton("Settings", Graphics::GetImageHandle(SettingsIcon), {32, 32})) {
            if(Ui::Screens::GetActiveScreen() == Ui::Screen::Settings) {
                Ui::Screens::SetActiveScreen(Ui::Screen::Media);
            } else {
                Ui::Screens::SetActiveScreen(Ui::Screen::Settings);
            }
        }
        ImGui::SameLine();
        RenderFps();
    }
    void ShutDown() {}
} // namespace GhostHunter::Ui::Screens::TopContent