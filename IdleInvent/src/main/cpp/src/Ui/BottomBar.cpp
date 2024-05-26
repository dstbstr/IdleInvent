#include "Ui/BottomBar.h"

#include "Ui/Ui.h"
#include "Ui/Screens.h"

#include "Graphics/Image.h"
#include "Platform/Graphics.h"

#include "imgui.h"

namespace {
    Image AchievementIcon;
    Image ResourcesIcon;
    Image StorageIcon;

    constexpr int IconSize = 128;
}

namespace Ui::BottomBar {
    bool Initialize() {
        return (Graphics::LoadImage("Icon/Achievements.png", AchievementIcon) &&
            Graphics::LoadImage("Icon/Resources.png", ResourcesIcon) && 
            Graphics::LoadImage("Icon/Storage.png", StorageIcon));
    }

    void Render() {
        auto spacing = ImGui::GetContentRegionAvail().x / 2;
        auto leftEdge = spacing / 2;

        ImGui::Begin("BottomBar", nullptr, BaseFlags);
        ImGui::SetCursorPosX(leftEdge);
        if (ImGui::ImageButton("ResourceIcon", (void*)(intptr_t)ResourcesIcon.TextureId, { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Resources);
        }
        ImGui::SameLine(spacing + leftEdge + IconSize);
        if (ImGui::ImageButton("StorageIcon", (void*)(intptr_t)StorageIcon.TextureId, { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Storages);
        }
        ImGui::SameLine(((spacing + IconSize) * 2) + leftEdge);
        if (ImGui::ImageButton("AchievementIcon", (void*)(intptr_t)AchievementIcon.TextureId, { IconSize, IconSize })) {
            Screens::SetActiveScreen(Screen::Achievements);
        }
        
        ImGui::End();
    }
}