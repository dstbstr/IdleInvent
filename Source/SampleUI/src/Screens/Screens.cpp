#include <SampleUI/Screens/Screens.h>

#include <SampleUI/Screens/Landing.h>
#include <SampleUI/Screens/SampleTreePanel.h>
#include <SampleUI/Screens/SampleParticles.h>

#include <Instrumentation/Logging.h>
#include <Ui/Dialog.h>
#include <Ui/Overlay.h>

namespace {
	auto activeScreenName = SampleUI::Screen::Landing;
	void (*activeScreenFn)() = SampleUI::Screens::Landing::Render;
} // namespace

namespace SampleUI {
	std::string ToString(Screen screen) {
		switch(screen) {
			using enum Screen;
			case Landing: return "Landing";
			case SampleTreePanel: return "SampleTreePanel";
            case SampleParticles: return "SampleParticles";
		}

		DR_ASSERT_MSG(false, "Invalid screen");
		return "Unknown screen";
	}

	namespace Screens {
		bool Initialize() {
			activeScreenName = Screen::Landing;
			activeScreenFn = Landing::Render;
			return ::Ui::Dialog::Initialize()
				&& Landing::Initialize()
				&& SampleTreePanel::Initialize()
				&& SampleParticles::Initialize();
		}

		void ShutDown() {
			Landing::ShutDown();
			SampleTreePanel::ShutDown();
			SampleParticles::ShutDown();
			::Ui::Dialog::ShutDown();
			activeScreenName = Screen::Landing;
			activeScreenFn = Landing::Render;
		}

		void Render() {
			activeScreenFn();
			::Ui::Dialog::Render();
			::Ui::Overlay::DrawFps();
		}

		void SetActiveScreen(Screen screen) {
			activeScreenName = screen;
			switch(screen) {
				using enum Screen;
				case Landing: activeScreenFn = Landing::Render; break;
				case SampleTreePanel: activeScreenFn = SampleTreePanel::Render; break;
                case SampleParticles: activeScreenFn = SampleParticles::Render; break;
			}
		}

		Screen GetActiveScreen() {
			return activeScreenName;
		}
	} // namespace Screens
} // namespace SampleUI::Ui
