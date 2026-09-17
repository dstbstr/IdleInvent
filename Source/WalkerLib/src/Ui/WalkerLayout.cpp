#include "Walker/Ui/WalkerLayout.h"
#include "Walker/Ui/WalkerContent.h"
#include "Walker/Ui/WalkerHeader.h"
#include "Walker/Ui/WalkerNav.h"

#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Ui/UiBuilder.h>
#include <Ui/UiUtil.h>

namespace Walker::WalkerUi::Layout {
    bool Initialize() { 
        InitializeFonts("DroidSans.ttf");

        bool success = true;
        success &= Content::Initialize();
        success &= Header::Initialize();
        success &= Nav::Initialize();
        DR_ASSERT(success);
        return success;
    }

    void Render() {
        auto width = Graphics::ScreenWidth;
        auto topHeight = Header::GetRequestedHeight(width);
        auto navHeight = Nav::GetRequestedHeight(width);
        auto mainHeight = std::max(0.f, Graphics::ScreenHeight - topHeight - navHeight);
        UiBuilder()
            .AddPart(topHeight, Header::Render)
            .AddPart(mainHeight, Content::Render)
            .AddPart(navHeight, Nav::Render)
            .Build();
    }
    
    void ShutDown() {
        Nav::ShutDown();
        Header::ShutDown();
        Content::ShutDown();
    }
} // namespace Walker::WalkerUi::Layout