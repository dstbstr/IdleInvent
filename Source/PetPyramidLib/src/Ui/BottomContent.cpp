#include "Pets/Ui/BottomContent.h"
#include "Pets/Ui/Screens/Screens.h"
#include "Pets/Ui/Ui.h"

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>

namespace {
    constexpr auto PetIcon = "Pets";
    constexpr auto CombatIcon = "Combat";
    constexpr auto BestiaryIcon = "Bestiary";
    constexpr auto RebirthIcon = "Rebirth";
} // namespace

namespace Pets::Ui::Screens::BottomContent {
    bool Initialize() {
        return Graphics::IsSpriteValid(PetIcon) && Graphics::IsSpriteValid(CombatIcon) &&
               Graphics::IsSpriteValid(BestiaryIcon) && Graphics::IsSpriteValid(RebirthIcon);
    }

    void ShutDown() {}

    void Render() {
        auto available = ImGui::GetContentRegionAvail();
        auto columnWidth = available.x / 4.f;
        auto framePadding = ImGui::GetStyle().FramePadding;
        auto minDim = std::min(available.y - framePadding.y * 2.f, columnWidth - framePadding.x * 2.f);
        auto iconSize = ImVec2(minDim, minDim);

        if(ImGui::BeginTable("##Navigation", 4, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings)) {
            auto AddButton = [&](const char* icon, Screen screen) {
                ImGui::TableNextColumn();
                if(SpriteButton(icon, Graphics::GetSprite(icon), iconSize)) {
                    Screens::SetActiveScreen(screen);
                }
            };

            AddButton(PetIcon, Screen::Pets);
            AddButton(CombatIcon, Screen::Combat);
            AddButton(BestiaryIcon, Screen::Bestiary);
            AddButton(RebirthIcon, Screen::Rebirth);

            ImGui::EndTable();
        }
    }
} // namespace Pets::Ui::Screens::BottomContent
