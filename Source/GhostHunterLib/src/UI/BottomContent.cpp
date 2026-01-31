#include "GhostHunter/Ui/BottomContent.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Ui/Screens/Screens.h"
#include <Ui/UiUtil.h>
#include <Platform/Graphics.h>

namespace {
    constexpr auto CoinIcon = "Icon/Coin.png";
    constexpr auto ToolsIcon = "Icon/Detector.png";
    constexpr auto InvestigateIcon = "Icon/Castle.png";
    constexpr auto EditIcon = "Icon/Journal.png";
    constexpr auto PrestigeIcon = "Icon/Tombstone.png";
    constexpr int IconSize = 64;
} // namespace

namespace GhostHunter::Ui::Screens::BottomContent {
    bool Initialize() {
        return Graphics::TryLoadImageFile(CoinIcon) && 
               Graphics::TryLoadImageFile(ToolsIcon) &&
               Graphics::TryLoadImageFile(InvestigateIcon) && 
               Graphics::TryLoadImageFile(EditIcon) &&
               Graphics::TryLoadImageFile(PrestigeIcon);
    }

    void ShutDown() {}

    void Render() {
        auto spacing = Graphics::ScreenWidth / 4;
        if(ImGui::ImageButton("CoinIcon", Graphics::GetImageHandle(CoinIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Market);
        }
        ImGui::SameLine(spacing);
        if(ImGui::ImageButton("InvestigateIcon", Graphics::GetImageHandle(InvestigateIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Investigate);
        }
        ImGui::SameLine(spacing * 2);
        if(ImGui::ImageButton("EditIcon", Graphics::GetImageHandle(EditIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Editing);
        }
        ImGui::SameLine(spacing * 3);
        if(ImGui::ImageButton("PrestigeIcon", Graphics::GetImageHandle(PrestigeIcon), {IconSize, IconSize})) {
            Screens::SetActiveScreen(Screen::Prestige);
        }
    }
}
