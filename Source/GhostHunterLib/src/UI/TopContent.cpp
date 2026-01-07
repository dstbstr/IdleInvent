#include "GhostHunter/Ui/TopContent.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/UI/Screens/Screens.h"
#include "GhostHunter/UI/Screens/SettingsScreen.h"
#include "GhostHunter/GameState/GameSettings.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "Ui/UiUtil.h"

#include <Platform/Graphics.h>
#include <DesignPatterns/ServiceLocator.h>

#include <imgui.h>
#include <format>

namespace {
    GhostHunter::GameSettings* gameSettings{nullptr};
    GhostHunter::GhostHunterResources* resources{nullptr};

    void RenderFps() {
        if(gameSettings->ShowFps) {
            const auto& frameRate = ImGui::GetIO().Framerate;
            TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
        } else {
            ImGui::Text("");
        }
    }

    void RenderResources() {
        auto resourceNames = GhostHunter::GetAllResourceNames();
        ImGui::BeginTable("Current Resources", 2);
        for(auto& name : resourceNames) {
            ImGui::TableNextColumn();
            ImGui::Text(GhostHunter::ToString(name).c_str());
            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(resources->GetResource(name)).c_str());
        }
        ImGui::EndTable();
    }
} // namespace

namespace GhostHunter::Ui::Screens::TopContent {
    constexpr auto SettingsIcon = "Icon/SettingsIcon64.png";
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        gameSettings = services.Get<GhostHunter::GameSettings>();
        resources = services.Get<GhostHunter::GhostHunterResources>();

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
        ImGui::PushFont(Ui::GetFont(Ui::FontSizes::H3));
        RenderResources();
        ImGui::PopFont();
    }
    void ShutDown() {}
} // namespace GhostHunter::Ui::Screens::TopContent