#include "Walker/Ui/WalkerNav.h"
#include "Walker/Ui/Screens/WalkerScreens.h"

#include <Utilities/EnumUtils.h>
#include <Ui/UiUtil.h>
#include <imgui.h>

#include <array>

namespace {
    auto TableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings;
}

namespace Walker::WalkerUi::Nav {
    bool Initialize() { return true; }
    void Render() { 
		auto screens = Enum::GetAllValues<Screen>();
        if(ImGui::BeginTable("##WalkerNav", static_cast<int>(screens.size() - 1), TableFlags)) {
            for(auto screen : screens) {
                if(screen == Screen::Settings) continue;

                ImGui::TableNextColumn();
                auto label = ToString(screen);
                ImGui::PushFont(GetFont(FontSizes::H2));

                if(ImGui::Button(label.c_str(), ImVec2{ImGui::GetContentRegionAvail().x, 0.f})) {
                    Screens::SetActiveScreen(screen);
                }
                ImGui::PopFont();
            }

            ImGui::EndTable();
        }
    }

    void ShutDown() {}

    f32 GetRequestedHeight(f32 availableWidth) { 
        auto& style = ImGui::GetStyle();
        return ImGui::GetFrameHeight() + style.CellPadding.y * 2.f + style.WindowPadding.y * 2.f;
    }
} // namespace Walker::WalkerUi::Nav