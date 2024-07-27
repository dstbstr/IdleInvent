#include "Ui/Splash.h"

#include "Platform/Graphics.h"
#include "Graphics/Image.h"

#include "EngineState.h"
#include "imgui.h"

namespace {
    Image SplashScreen;
}

namespace Ui::Splash {
    bool Initialize() {
        return Graphics::LoadImage("SplashScreen.jpg", SplashScreen);
    }

    void ShutDown() {
        Graphics::UnloadImage(SplashScreen);
    }

    void Render() {        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(Graphics::ScreenWidth, Graphics::ScreenHeight));

        ImGui::Begin("SplashScreen", nullptr, ImageFlags);
        ImGui::Image(SplashScreen.ToHandle(), ImGui::GetContentRegionAvail());
        ImGui::End();
    }
}