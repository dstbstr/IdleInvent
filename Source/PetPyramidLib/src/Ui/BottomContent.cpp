#include "Pets/Ui/BottomContent.h"
#include "Pets/Ui/Screens/Screens.h"
#include "Pets/Ui/Ui.h"

#include <Platform/Graphics.h>

namespace {
    constexpr auto PetIcon = "Pets";
    constexpr auto CombatIcon = "Combat";
    constexpr auto BestiaryIcon = "Bestiary";
    constexpr auto RebirthIcon = "Rebirth";
    constexpr int IconSize = 64;
} // namespace

namespace Pets::Ui::Screens::BottomContent {
    bool Initialize() {
        return Graphics::IsSpriteValid(PetIcon) && Graphics::IsSpriteValid(CombatIcon) &&
               Graphics::IsSpriteValid(BestiaryIcon) && Graphics::IsSpriteValid(RebirthIcon);
    }

    void ShutDown() {}

    void Render() {
        auto spacing = Graphics::ScreenWidth / 4;
        auto petSprite = Graphics::GetSprite(PetIcon);
        if(ImGui::ImageButton("PetIcon", petSprite.Texture, {IconSize, IconSize}, petSprite.UvMin, petSprite.UvMax)) {
            Screens::SetActiveScreen(Screen::Pets);
        }
        ImGui::SameLine(spacing);
        auto combatSprite = Graphics::GetSprite(CombatIcon);
        if(ImGui::ImageButton(
               "CombatIcon", combatSprite.Texture, {IconSize, IconSize}, combatSprite.UvMin, combatSprite.UvMax
           )) {
            Screens::SetActiveScreen(Screen::Combat);
        }
        ImGui::SameLine(spacing * 2);
        auto bestiaryIcon = Graphics::GetSprite(BestiaryIcon);
        if(ImGui::ImageButton(
               "BestiaryIcon", bestiaryIcon.Texture, {IconSize, IconSize}, bestiaryIcon.UvMin, bestiaryIcon.UvMax
           )) {
            Screens::SetActiveScreen(Screen::Bestiary);
        }
        ImGui::SameLine(spacing * 3);
        auto rebirthIcon = Graphics::GetSprite(RebirthIcon);
        if(ImGui::ImageButton(
               "RebirthIcon", rebirthIcon.Texture, {IconSize, IconSize}, rebirthIcon.UvMin, rebirthIcon.UvMax
           )) {
            Screens::SetActiveScreen(Screen::Rebirth);
        }
    }
} // namespace Pets::Ui::Screens::BottomContent
