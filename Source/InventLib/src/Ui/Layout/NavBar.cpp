#include "Invent/Ui/Layout/NavBar.h"
#include "Invent/Ui/Ui.h"
#include "Invent/Ui/Screens/Screens.h"

#include <Platform/Graphics.h>
#include <imgui.h>
#include <string>

namespace {
    constexpr auto AchievementIcon = "Achievement";
    constexpr auto HomeIcon = "Home";
    constexpr auto BuildingIcon = "Building";
    constexpr auto ExploreIcon = "Explore";
    constexpr auto ResearchIcon = "Research";
    constexpr int IconSize = 64;
}

namespace Ui::NavBar {
    bool Initialize() {
        return Graphics::IsSpriteValid(AchievementIcon) &&
               Graphics::IsSpriteValid(HomeIcon) &&
               Graphics::IsSpriteValid(BuildingIcon) &&
               Graphics::IsSpriteValid(ExploreIcon) &&
               Graphics::IsSpriteValid(ResearchIcon);
    }

    void ShutDown() {
    }
    
    void Render() {
        auto spacing = Graphics::ScreenWidth / 5;
        //auto spacing = 200.0F;
        //ImGui::Begin("NavBar", nullptr, BaseFlags | ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Begin("NavBar", nullptr, BaseFlags);
 
        auto homeIcon = Graphics::GetSprite(HomeIcon);
        if(ImGui::ImageButton("HomeIcon", homeIcon.Texture, { IconSize, IconSize }, homeIcon.UvMin, homeIcon.UvMax)) {
            Screens::SetActiveScreen(Screen::Home);
        }
        ImGui::SameLine(spacing);

        auto researchIcon = Graphics::GetSprite(ResearchIcon);
        if(ImGui::ImageButton("ResearchIcon", researchIcon.Texture, { IconSize, IconSize }, researchIcon.UvMin, researchIcon.UvMax)) {
            Screens::SetActiveScreen(Screen::Research);
        }
        ImGui::SameLine(spacing * 2);
        auto buildingIcon = Graphics::GetSprite(BuildingIcon);
        if(ImGui::ImageButton("BuildIcon", buildingIcon.Texture, { IconSize, IconSize }, buildingIcon.UvMin, buildingIcon.UvMax)) {
            Screens::SetActiveScreen(Screen::Build);
        }
        ImGui::SameLine(spacing * 3);
        auto exploreIcon = Graphics::GetSprite(ExploreIcon);
        if(ImGui::ImageButton("ExploreIcon", exploreIcon.Texture, { IconSize, IconSize }, exploreIcon.UvMin, exploreIcon.UvMax)) {
            Screens::SetActiveScreen(Screen::Explore);
        }
        ImGui::SameLine(spacing * 4);
        auto achievementIcon = Graphics::GetSprite(AchievementIcon);
        if(ImGui::ImageButton("AchievementIcon", achievementIcon.Texture, { IconSize, IconSize }, achievementIcon.UvMin, achievementIcon.UvMax)) {
            Screens::SetActiveScreen(Screen::Achievements);
        }

        ImGui::End();
    }
}