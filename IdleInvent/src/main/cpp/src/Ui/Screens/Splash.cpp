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
    EngineState* Engine{nullptr};

    Image SplashScreen;
}

namespace Ui::Screens::Splash {
    bool Initialize() {
        Engine = &ServiceLocator::Get().GetOrCreate<EngineState>();
        return Graphics::LoadImage("SplashScreen.jpg", SplashScreen);
    }

    void ShutDown() {
        Graphics::UnloadImage(SplashScreen);
    }

    void Render() {
        using namespace std::chrono_literals;

        bool closeSplash = Engine->FrameNum > 1;
#ifndef DEBUG
        closeSplash = Engine->ElapsedTime >= 2s;
#endif
        if(closeSplash) {
            Screens::SetActiveScreen(Screen::Home);
            return;
        }   

        ImGui::Begin("SplashScreen", nullptr, ImageFlags);
        ImGui::Image((void*)(intptr_t)SplashScreen.TextureId, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
}