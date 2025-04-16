#include "InventLib/Ui/Layout/NavBar.h"
#include "InventLib/Ui/Ui.h"
#include "InventLib/Ui/Screens/Screens.h"

#include <Platform/Image.h>
#include <Platform/Graphics.h>
#include <imgui.h>
#include <string>

namespace {
    constexpr auto AchievementIcon = "Icon/AchievementIcon64.png";
    constexpr auto HomeIcon = "Icon/HomeIcon64.png";
    constexpr auto BuildingIcon = "Icon/BuildingIcon64.png";
    constexpr auto ExploreIcon = "Icon/ExploreIcon64.png";
    constexpr auto ResearchIcon = "Icon/ResearchIcon64.png";
    //constexpr int IconSize = 96;
    constexpr int IconSize = 64;
}

namespace Ui::NavBar {
    bool Initialize() {
        return Graphics::TryLoadImageFile(AchievementIcon) &&
               Graphics::TryLoadImageFile(HomeIcon) &&
               Graphics::TryLoadImageFile(BuildingIcon) &&
               Graphics::TryLoadImageFile(ExploreIcon) &&
               Graphics::TryLoadImageFile(ResearchIcon);
    }

    void ShutDown() {
    }
    
    void Render() {
        auto spacing = Graphics::ScreenWidth / 5;
        //auto spacing = 200.0F;
        //ImGui::Begin("NavBar", nullptr, BaseFlags | ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Begin("NavBar", nullptr, BaseFlags);
        if(ImGui::ImageButton("HomeIcon", Graphics::GetImageHandle(HomeIcon), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Home);
        }
        ImGui::SameLine(spacing);
        if(ImGui::ImageButton("ResearchIcon", Graphics::GetImageHandle(ResearchIcon), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Research);
        }
        ImGui::SameLine(spacing * 2);
        if(ImGui::ImageButton("BuildIcon", Graphics::GetImageHandle(BuildingIcon), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Build);
        }
        ImGui::SameLine(spacing * 3);
        if(ImGui::ImageButton("ExploreIcon", Graphics::GetImageHandle(ExploreIcon), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Explore);
        }
        ImGui::SameLine(spacing * 4);
        if(ImGui::ImageButton("AchievementIcon", Graphics::GetImageHandle(AchievementIcon), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Achievements);
        }
        /*
    ImGui::SameLine(spacing * 5);
        if(ImGui::ImageButton("SettingsIcon", SettingsIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Settings);
        }
        */

        /*
        ImGui::BeginTable("NavTable", 4, ImGuiTableFlags_SizingStretchSame);
        ImGui::TableNextColumn();
        if (ImGui::ImageButton("HomeIcon", HomeIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Home);
        }
        ImGui::TableNextColumn();
        if(ImGui::ImageButton("PurchasesIcon", PurchasesIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Purchases);
        }

        ImGui::TableNextColumn();
        if (ImGui::ImageButton("AchievementIcon", AchievementIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Achievements);
        }

        ImGui::TableNextColumn();
        if(ImGui::ImageButton("SettingsIcon", SettingsIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Settings);
        }

        ImGui::EndTable();
        */
        ImGui::End();
    }
}