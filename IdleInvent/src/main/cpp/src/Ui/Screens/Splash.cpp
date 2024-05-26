#include "Ui/Screens/Splash.h"

#include "Ui/Screens.h"
#include "Platform/Graphics.h"
#include "Graphics/Image.h"

#include "EngineState.h"
#include "imgui.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"

#include <chrono>

namespace {
    Image SplashScreen;
}

namespace Ui::Screens::Splash {
    void Render() {
        using namespace std::chrono_literals;

        auto& engineState = ServiceLocator::Get().GetRequired<EngineState>();
        static bool initialized = [](){
            Graphics::LoadImage("SplashScreen.jpg", SplashScreen);
            return true;
        }();
        DR_ASSERT_MSG(initialized, "Failed to load splash screen image");

        bool closeSplash = engineState.FrameNum > 1;
#ifndef DEBUG
        closeSplash = engineState.ElapsedTime >= 2s;
#endif
        if(closeSplash) {
            Screens::SetActiveScreen(Screen::Resources);
            return;
        }   

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        auto screenPair = Graphics::GetScreenSize();
        auto screenSize = ImVec2(screenPair.first, screenPair.second);
        ImGui::SetNextWindowSize(screenSize);

        ImGui::Begin("SplashScreen", nullptr, ImageFlags);
        ImGui::Image((void*)(intptr_t)SplashScreen.TextureId, screenSize);
        ImGui::End();
    }
}