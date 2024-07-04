#include "Ui/Splash.h"

#include "Ui/Screens/Screens.h"
#include "Platform/Graphics.h"
#include "Graphics/Image.h"

#include "EngineState.h"
#include "imgui.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"

#include <chrono>

namespace {
    float width = 0;
    float height = 0;

    EngineState* Engine{nullptr};

    Image SplashScreen;
}

namespace Ui::Splash {
    bool Initialize() {
        Engine = &ServiceLocator::Get().GetOrCreate<EngineState>();
        auto [w, h] = Graphics::GetScreenSize();
        width = static_cast<float>(w);
        height = static_cast<float>(h);

        return Graphics::LoadImage("SplashScreen.jpg", SplashScreen);
    }

    void ShutDown() {
        Graphics::UnloadImage(SplashScreen);
    }

    void Render() {
        using namespace std::chrono_literals;

        if(Engine->FrameNum == 2) {
            // initialize the rest of the UI after splash screen is visible
            Ui::Initialize();
        }

        bool closeSplash = Engine->FrameNum > 2;
#ifndef DEBUG
        closeSplash = Engine->ElapsedTime >= 2s;
#endif
        if(closeSplash) {
            Screens::SetActiveScreen(Screen::StartLife);
            return;
        }   
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(width, height));

        ImGui::Begin("SplashScreen", nullptr, ImageFlags);
        ImGui::Image((void*)(intptr_t)SplashScreen.TextureId, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
}