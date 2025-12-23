#include "GhostHunter/Ui/BottomContent.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/UI/Screens/Screens.h"
#include <Ui/UiUtil.h>
#include <Platform/Graphics.h>


#include <imgui.h>

namespace {
    constexpr auto CoinIcon = "Icon/Coin.png";
    constexpr auto ToolsIcon = "Icon/Detector.png";
    constexpr auto LocationIcon = "Icon/Castle.png";
    constexpr auto JournalIcon = "Icon/Journal.png";
    constexpr auto PrestigeIcon = "Icon/Tombstone.png";
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
        if(ImGui::ImageButton("CoinIcon", Graphics::GetImageHandle(CoinIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Media);
        }
        ImGui::SameLine(spacing);
        if(ImGui::ImageButton("ToolsIcon", Graphics::GetImageHandle(ToolsIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Tools);
        }
        ImGui::SameLine(spacing * 2);
        if(ImGui::ImageButton("LocationIcon", Graphics::GetImageHandle(LocationIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Location);
        }
        ImGui::SameLine(spacing * 3);
        if(ImGui::ImageButton("JournalIcon", Graphics::GetImageHandle(JournalIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Journal);
        }
        ImGui::SameLine(spacing * 4);
        if(ImGui::ImageButton("PrestigeIcon", Graphics::GetImageHandle(PrestigeIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Prestige);
        }
        ImGui::End();
    }
}
