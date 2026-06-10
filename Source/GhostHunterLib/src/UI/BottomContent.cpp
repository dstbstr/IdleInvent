#include "GhostHunter/Ui/BottomContent.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Ui/Screens/Screens.h"
#include <Ui/UiUtil.h>
#include <Platform/Graphics.h>

namespace {
    constexpr auto CoinIcon = "Coin";
    constexpr auto ToolsIcon = "Detector";
    constexpr auto InvestigateIcon = "Castle";
    constexpr auto EditIcon = "Journal";
    constexpr auto PrestigeIcon = "Tombstone";
    constexpr int IconSize = 64;
} // namespace

namespace GhostHunter::Ui::Screens::BottomContent {
    bool Initialize() {
        return Graphics::IsSpriteValid(CoinIcon) && 
               Graphics::IsSpriteValid(ToolsIcon) &&
               Graphics::IsSpriteValid(InvestigateIcon) && 
               Graphics::IsSpriteValid(EditIcon) &&
               Graphics::IsSpriteValid(PrestigeIcon);
    }

    void ShutDown() {}

    void Render() {
        auto spacing = Graphics::ScreenWidth / 4;
        auto coinSprite = Graphics::GetSprite(CoinIcon);
        if(ImGui::ImageButton("CoinIcon", coinSprite.Texture, {IconSize, IconSize}, coinSprite.UvMin, coinSprite.UvMax)) {
            Screens::SetActiveScreen(Screen::Market);
        }
        ImGui::SameLine(spacing);
        auto investigateSprite = Graphics::GetSprite(InvestigateIcon);
        if(ImGui::ImageButton("InvestigateIcon", investigateSprite.Texture, {IconSize, IconSize}, investigateSprite.UvMin, investigateSprite.UvMax)) {
            Screens::SetActiveScreen(Screen::Investigate);
        }
        ImGui::SameLine(spacing * 2);
        auto editSprite = Graphics::GetSprite(EditIcon);
        if(ImGui::ImageButton("EditIcon", editSprite.Texture, {IconSize, IconSize}, editSprite.UvMin, editSprite.UvMax)) {
            Screens::SetActiveScreen(Screen::Editing);
        }
        ImGui::SameLine(spacing * 3);
        auto prestigeSprite = Graphics::GetSprite(PrestigeIcon);
        if(ImGui::ImageButton("PrestigeIcon", prestigeSprite.Texture, {IconSize, IconSize}, prestigeSprite.UvMin, prestigeSprite.UvMax)) {
            Screens::SetActiveScreen(Screen::Prestige);
        }
    }
}
