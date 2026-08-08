#include "Invent/Ui/Layout/Layout.h"
#include "Invent/Ui/Layout/EmptyBar.h"
#include "Invent/Ui/Layout/NavBar.h"
#include "Invent/Ui/Layout/NewsFeed.h"
#include "Invent/Ui/Layout/Popups.h"
#include "Invent/Ui/Layout/PrimaryResource.h"
#include "Invent/Ui/Layout/ResourceBar.h"
#include "Invent/Ui/Layout/TopBar.h"
#include "Invent/Ui/Screens/Screens.h"

#include <Platform/Graphics.h>
#include <Instrumentation/Logging.h>
#include <Ui/UiBuilder.h>
#include <imgui.h>

#include <functional>
#include <vector>

namespace Ui::Layout {
    bool Initialize() {
        return EmptyBar::Initialize() &&
                NavBar::Initialize() &&
                NewsFeed::Initialize() &&
                Popups::Initialize() &&
                PrimaryResource::Initialize() &&
                ResourceBar::Initialize() &&
                TopBar::Initialize() &&
                Screens::Initialize();
    }

    void ShutDown() {
        Screens::ShutDown();
        EmptyBar::ShutDown();
        NavBar::ShutDown();
        NewsFeed::ShutDown();
        Popups::ShutDown();
        PrimaryResource::ShutDown();
        ResourceBar::ShutDown();
        TopBar::ShutDown();
    }

    void Render() {
        auto screenHeight = Graphics::ScreenHeight;
        UiBuilder()
            .AddPart(screenHeight * 0.03F, EmptyBar::Render)
            .AddPart(screenHeight * 0.08F, TopBar::Render)
            .AddPart(screenHeight * 0.65F, Screens::Render)
            .AddPart(screenHeight * 0.07F, ResourceBar::Render)
            .AddPart(screenHeight * 0.065F, NavBar::Render)
            .AddPart(screenHeight * 0.08F, PrimaryResource::Render)
            .Build();
    }
}