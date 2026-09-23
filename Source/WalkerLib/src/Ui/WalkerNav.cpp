#include "Walker/Ui/WalkerNav.h"
#include "Walker/Ui/Screens/WalkerScreens.h"

#include <array>
#include <imgui.h>

namespace {
    auto TableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings;
}

namespace Walker::WalkerUi::Nav {
    bool Initialize() { return true; }
    void Render() { 
        constexpr std::array screens{Screen::Travel, Screen::Store, Screen::Crew, Screen::Rebirth};

        if(ImGui::BeginTable("##WalkerNav", static_cast<int>(screens.size()), TableFlags)) {
            for(auto screen : screens) {
                ImGui::TableNextColumn();
                auto label = ToString(screen);

                if(ImGui::Button(label.c_str(), ImVec2{ImGui::GetContentRegionAvail().x, 0.f})) {
                    Screens::SetActiveScreen(screen);
                }
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