#include "Ui/NavBar.h"

#include "Ui/Ui.h"
#include "Ui/Screens.h"

#include "Graphics/Image.h"
#include "Platform/Graphics.h"

#include "imgui.h"

namespace {
    Image AchievementIcon;
    Image HomeIcon;
    Image PurchasesIcon;
    Image SettingsIcon;

    constexpr int IconSize = 64;
}

namespace Ui::NavBar {
    bool Initialize() {
        return Graphics::LoadImage("Icon/Achievements.png", AchievementIcon) &&
            Graphics::LoadImage("Icon/Resources.png", HomeIcon) && 
            Graphics::LoadImage("Icon/Purchases.png", PurchasesIcon) &&
            Graphics::LoadImage("Icon/Settings.png", SettingsIcon);
    }

    void ShutDown() {
        Graphics::UnloadImage(AchievementIcon);
        Graphics::UnloadImage(HomeIcon);
        Graphics::UnloadImage(PurchasesIcon);
        Graphics::UnloadImage(SettingsIcon);
    }
    
    void Render() {
        ImGui::Begin("NavBar", nullptr, BaseFlags);

        ImGui::BeginTable("NavTable", 4, ImGuiTableFlags_SizingStretchSame);
        ImGui::TableNextColumn();
        if (ImGui::ImageButton("HomeIcon", (void*)(intptr_t)HomeIcon.TextureId, { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Home);
        }
        ImGui::TableNextColumn();
        if (ImGui::ImageButton("AchievementIcon", (void*)(intptr_t)AchievementIcon.TextureId, { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Achievements);
        }

        ImGui::TableNextColumn();
        if(ImGui::ImageButton("PurchasesIcon", (void*)(intptr_t)PurchasesIcon.TextureId, { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Purchases);
        }

        ImGui::TableNextColumn();
        if(ImGui::ImageButton("SettingsIcon", (void*)(intptr_t)SettingsIcon.TextureId, { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Settings);
        }

        ImGui::EndTable();

        ImGui::End();
    }
}