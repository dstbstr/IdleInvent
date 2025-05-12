#include "InventLib/Ui/Splash.h"
#include "InventLib/Ui/Ui.h"

#include "Instrumentation/Logging.h"
#include <Platform/Graphics.h>
#include <EngineState.h>
#include <imgui.h>

namespace {
    //Image SplashScreen;
    constexpr auto SplashScreen = "SplashScreen.jpg";
}

namespace Ui::Splash {
    bool Initialize() {
        return Graphics::TryLoadImageFile(SplashScreen);
        //return Graphics::LoadImageFile("Background/StoneAge.png", SplashScreen);
    }

    void ShutDown() {
        //Graphics::UnloadImage(SplashScreen);
    }

    void Render() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(Graphics::ScreenWidth, Graphics::ScreenHeight));

        ImGui::Begin("SplashScreen", nullptr, ImageFlags);
        //ImGui::Image(SplashScreen.ToHandle(), ImGui::GetContentRegionAvail());
        ImGui::Image(Graphics::GetImageHandle(SplashScreen), ImGui::GetContentRegionAvail());
        ImGui::End();
    }
}