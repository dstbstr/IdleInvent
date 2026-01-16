#include "GhostHunter/Ui/TopContent.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Ui/Screens/Screens.h"
#include "GhostHunter/Ui/Screens/SettingsScreen.h"
#include "GhostHunter/GameState/GameSettings.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "Ui/UiUtil.h"

#include <Platform/Graphics.h>
#include <Resources/Resource.h>
#include <DesignPatterns/ServiceLocator.h>

#include <imgui.h>
#include <format>

namespace {
    GhostHunter::GameSettings* gameSettings{nullptr};
    ResourceCollection* resources{nullptr};

    void RenderFps() {
        if(gameSettings->ShowFps) {
            const auto& frameRate = ImGui::GetIO().Framerate;
            TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
        } else {
            ImGui::Text("");
        }
    }

    void RenderResources() {
        ImGui::BeginTable("Current Resources", 2);
        for(const auto& [id, resource] : *resources) {
            ImGui::TableNextColumn();
            ImGui::Text("%s", resource.Name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", std::to_string(resource.Current).c_str());
        }
        ImGui::EndTable();
    }
} // namespace

namespace GhostHunter::Ui::Screens::TopContent {
    constexpr auto SettingsIcon = "Icon/SettingsIcon64.png";
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        gameSettings = services.Get<GhostHunter::GameSettings>();
        resources = services.Get<ResourceCollection>();

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