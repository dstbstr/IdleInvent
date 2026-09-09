#include "Pets/Ui/Ui.h"
#include "Pets/Ui/TopContent.h"
#include "Pets/Ui/MainContent.h"
#include "Pets/Ui/BottomContent.h"

#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Ui/UiBuilder.h>
#include <Ui/UiUtil.h>

namespace {
    constexpr auto ButtonSprites = "Icons/Buttons.png";
    constexpr auto ItemSprites = "Icons/Items.png";
} // namespace

namespace Pets::Ui {
	bool Initialize() {
		InitializeFonts("DroidSans.ttf");
		
        DR_ASSERT(Graphics::TryLoadSpriteSheet(ButtonSprites));
        DR_ASSERT(Graphics::TryLoadSpriteSheet(ItemSprites));
        DR_ASSERT(Screens::BottomContent::Initialize());
        DR_ASSERT(Screens::MainContent::Initialize());
        DR_ASSERT(Screens::TopContent::Initialize());

        return true;
    }

	void Render() {
        auto width = Graphics::ScreenWidth;
        auto topHeight = Screens::TopContent::GetRequestedHeight(width);
        auto bottomHeight = Screens::BottomContent::GetRequestedHeight(width);
        auto mainHeight = std::max(0.f, Graphics::ScreenHeight - topHeight - bottomHeight);
		UiBuilder()
			.AddPart(topHeight, Screens::TopContent::Render)
			.AddPart(mainHeight, Screens::MainContent::Render)
			.AddPart(bottomHeight, Screens::BottomContent::Render)
			.Build();
	}

	void ShutDown() {
		Screens::TopContent::ShutDown();
		Screens::MainContent::ShutDown();
		Screens::BottomContent::ShutDown();
	}

    ImFont* GetFont(FontSizes fontSize) {
        switch(fontSize) {
            using enum FontSizes;
        case H1: return Graphics::GetFont("H1");
        case H2: return Graphics::GetFont("H2");
        case H3: return Graphics::GetFont("H3");
        case H4: return Graphics::GetFont("H4");
        default: return nullptr;
        }
    }

} // namespace Pets::Ui
