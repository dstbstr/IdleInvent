#include "GhostHunter/Ui/BottomContent.h"
#include "GhostHunter/Ui/Ui.h"
#include <Ui/UiUtil.h>
#include <Platform/Graphics.h>


#include <imgui.h>

namespace {
    constexpr auto CoinIcon = "Icon/CoinIcon64.png";
    constexpr auto ToolsIcon = "Icon/ToolsIcon64.png";
    constexpr auto LocationIcon = "Icon/LocationIcon64.png";
    constexpr auto JournalIcon = "Icon/JournalIcon64.png";
    constexpr auto PrestigeIcon = "Icon/PrestigeIcon64.png";
    constexpr int IconSize = 64;
} // namespace

namespace GhostHunter::Ui::Screens::BottomContent {
    bool Initialize() {
        return Graphics::TryLoadImageFile(CoinIcon) && 
               Graphics::TryLoadImageFile(ToolsIcon) &&
               Graphics::TryLoadImageFile(LocationIcon) && 
               Graphics::TryLoadImageFile(JournalIcon) &&
               Graphics::TryLoadImageFile(PrestigeIcon);
    }

    void ShutDown() {}

    void Render() {
        auto spacing = Graphics::ScreenWidth / 5;
        // auto spacing = 200.0F;
        // ImGui::Begin("NavBar", nullptr, BaseFlags | ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Begin("NavBar", nullptr, BaseFlags);
        if(ImGui::ImageButton("HomeIcon", Graphics::GetImageHandle(HomeIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Home);
        }
        ImGui::SameLine(spacing);
        if(ImGui::ImageButton("ResearchIcon", Graphics::GetImageHandle(ResearchIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Research);
        }
        ImGui::SameLine(spacing * 2);
        if(ImGui::ImageButton("BuildIcon", Graphics::GetImageHandle(BuildingIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Build);
        }
        ImGui::SameLine(spacing * 3);
        if(ImGui::ImageButton("ExploreIcon", Graphics::GetImageHandle(ExploreIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Explore);
        }
        ImGui::SameLine(spacing * 4);
        if(ImGui::ImageButton("AchievementIcon", Graphics::GetImageHandle(AchievementIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Achievements);
        }
        ImGui::End();
    }
}
