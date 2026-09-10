#include "Pets/Ui/BottomContent.h"
#include "Pets/Ui/Screens/Screens.h"
#include "Pets/Ui/Ui.h"

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

#include <array>

namespace {
    using namespace Pets::Ui;
    constexpr std::array<std::pair<const char*, Screen>, 5> Icons{{
         {"Pets", Screen::Pets},
         {"Combat", Screen::Combat},
         {"Store", Screen::Store},
         {"Bestiary", Screen::Bestiary},
         {"Rebirth", Screen::Rebirth}
    }};

    f32 GetNavButtonSize(f32 availableWidth) { 
        auto& style = ImGui::GetStyle();
        auto contentWidth = availableWidth - style.WindowPadding.x * 2.f;
        auto columnWidth = contentWidth / static_cast<f32>(Icons.size());

        return columnWidth - style.CellPadding.x * 2.f - style.FramePadding.x * 2.f;
    }

} // namespace

namespace Pets::Ui::Screens::BottomContent {
    bool Initialize() {
        bool valid = true;
        for(const auto& [icon, screen] : Icons) {
            valid &= Graphics::IsSpriteValid(icon);
        }

        return valid;
    }

    void ShutDown() {}

    f32 GetRequestedHeight(f32 availableWidth) { 
        auto& style = ImGui::GetStyle();
        auto buttonSize = GetNavButtonSize(availableWidth);

        return buttonSize + 
            style.FramePadding.y * 2.f + 
            style.CellPadding.y * 2.f +
            style.WindowPadding.y * 2.f;
    }

    void Render() {
        auto iconDim = GetNavButtonSize(ImGui::GetContentRegionAvail().x);
        auto iconSize = ImVec2(iconDim, iconDim);

        if(ImGui::BeginTable("##Navigation", Icons.size(), ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings)) {
            for(const auto& [icon, screen] : Icons) {
                ImGui::TableNextColumn();
                if(SpriteButton(icon, Graphics::GetSprite(icon), iconSize)) {
                    Screens::SetActiveScreen(screen);
                }
            }

            ImGui::EndTable();
        }
    }
} // namespace Pets::Ui::Screens::BottomContent
