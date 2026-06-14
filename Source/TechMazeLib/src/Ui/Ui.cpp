#include "TechMaze/Ui/Ui.h"
#include "TechMaze/Ui/TopContent.h"
#include "TechMaze/Ui/MainContent.h"
#include "TechMaze/Ui/BottomContent.h"

#include "Platform/Graphics.h"
#include "Instrumentation/Logging.h"
#include "Ui/UiBuilder.h"
#include <functional>
#include <string>

namespace TechMaze::Ui {
    bool Initialize() {
        InitializeFonts("DroidSans.ttf");
        return true;
    }
    void Render() {
        auto screenHeight = Graphics::ScreenHeight;
        UiBuilder()
            .AddPart(screenHeight * 0.05f, Screens::TopContent::Render)
            .AddPart(screenHeight * 0.75f, Screens::MainContent::Render)
            .AddPart(screenHeight * 0.2f, Screens::BottomContent::Render)
            .Build();
    }

}
