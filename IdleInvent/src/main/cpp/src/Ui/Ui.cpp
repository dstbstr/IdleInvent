#include "Ui/Ui.h"

#include "Platform/Graphics.h"
#include "Ui/BottomBar.h"
#include "Ui/Screens.h"
#include "Ui/TopBar.h"
#include "EngineState.h"

#include "Core/Instrumentation/Logging.h"
#include "imgui.h"

#include <thread>

namespace Ui {
    void Render() {
        if(Screens::GetActiveScreen() == Screen::Splash) {
            if(ServiceLocator::Get().GetRequired<EngineState>().FrameNum == 2) {
                static auto initialized = []() {
                    return Screens::InitializeScreens() &&
                    TopBar::Initialize() &&
                    BottomBar::Initialize();
                }();
                DR_ASSERT_MSG(initialized, "Failed to initialize UI");
            }
            Ui::Screens::RenderActiveScreen();
            return;
        }

        auto [width, height] = Graphics::GetScreenSize();
        auto topBorderHeight = height / 5;
        auto topBorderPos = topBorderHeight;
        auto bottomBorderHeight = height / 5;
        auto bottomBorderPos = height - bottomBorderHeight;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(width, topBorderHeight));
        TopBar::Render();

        ImGui::SetNextWindowPos(ImVec2(0, topBorderPos));
        ImGui::SetNextWindowSize(ImVec2(width, height - bottomBorderHeight - topBorderHeight));
        Screens::RenderActiveScreen();

        ImGui::SetNextWindowPos(ImVec2(0, bottomBorderPos));
        ImGui::SetNextWindowSize(ImVec2(width, bottomBorderHeight));
        BottomBar::Render();

        // static bool WantTextInputLast = false;
        // if(io.WantTextInput && !WantTextInputLast) { ShowSoftKeyboardInput(); }
        // WantTextInputLast = io.WantTextInput;
    }

} // namespace Ui