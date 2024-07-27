#include "Ui/Layout/NavBar.h"

#include "Ui/Ui.h"
#include "Ui/Screens/Screens.h"

#include "Graphics/Image.h"
#include "Platform/Graphics.h"

#include "imgui.h"

namespace {
    Image AchievementIcon;
    Image HomeIcon;
    //Image PurchasesIcon;
    //Image SettingsIcon;
    Image BuildIcon;
    Image ExploreIcon;
    Image ResearchIcon;

    constexpr int IconSize = 96;
}

namespace Ui::NavBar {
    bool Initialize() {
        return Graphics::LoadImage("Icon/AchievementIcon64.png", AchievementIcon) &&
            Graphics::LoadImage("Icon/HomeIcon64.png", HomeIcon) &&
            //Graphics::LoadImage("Icon/Purchases.png", PurchasesIcon) &&
            //Graphics::LoadImage("Icon/Settings.png", SettingsIcon) &&
            Graphics::LoadImage("Icon/BuildingIcon64.png", BuildIcon) &&
            Graphics::LoadImage("Icon/ExploreIcon64.png", ExploreIcon) &&
            Graphics::LoadImage("Icon/ResearchIcon64.png", ResearchIcon);
    }

    void ShutDown() {
        Graphics::UnloadImage(AchievementIcon);
        Graphics::UnloadImage(HomeIcon);
        //Graphics::UnloadImage(PurchasesIcon);
        //Graphics::UnloadImage(SettingsIcon);
        Graphics::UnloadImage(BuildIcon);
        Graphics::UnloadImage(ExploreIcon);
        Graphics::UnloadImage(ResearchIcon);
    }
    
    void Render() {
        auto spacing = 200.0F;
        //ImGui::Begin("NavBar", nullptr, BaseFlags | ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Begin("NavBar", nullptr, BaseFlags);
        if(ImGui::ImageButton("HomeIcon", HomeIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Home);
        }
        ImGui::SameLine(spacing);
        if(ImGui::ImageButton("ResearchIcon", ResearchIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Research);
        }
        ImGui::SameLine(spacing * 2);
        if(ImGui::ImageButton("BuildIcon", BuildIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Build);
        }
        ImGui::SameLine(spacing * 3);
        if(ImGui::ImageButton("ExploreIcon", ExploreIcon.ToHandle(), { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Explore);
        }
        ImGui::SameLine(spacing * 4);
        if(ImGui::ImageButton("AchievementIcon", AchievementIcon.ToHandle(), { IconSize, IconSize })) {
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