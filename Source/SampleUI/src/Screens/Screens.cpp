#include <SampleUI/Screens/Screens.h>

#include <SampleUI/Screens/Landing.h>
#include <SampleUI/Screens/SampleCombat.h>
#include <SampleUI/Screens/SampleGiantMap.h>
#include <SampleUI/Screens/SampleNav.h>
#include <SampleUI/Screens/SampleParticles.h>
#include <SampleUI/Screens/SampleSimpleMap.h>
#include <SampleUI/Screens/SampleTreePanel.h>

#include <Instrumentation/Logging.h>
#include <Ui/Dialog.h>
#include <Ui/Overlay.h>

#include <array>

namespace {
	using namespace SampleUI;

	constexpr std::array Definitions{
        ScreenDefinition{
            .Id = Screen::Landing, 
            .Name = "Landing", 
            .Render = Screens::Landing::Render
        },
        ScreenDefinition{
            .Id = Screen::SampleTreePanel,
            .Name = "SampleTreePanel",
            .LandingLabel = "Tree\nPanel",
            .Initialize = Screens::SampleTreePanel::Initialize,
            .ShutDown = Screens::SampleTreePanel::ShutDown,
            .Render = Screens::SampleTreePanel::Render
        },
        ScreenDefinition{
            .Id = Screen::SampleParticles,
            .Name = "SampleParticles",
            .LandingLabel = "Particles",
            .Initialize = Screens::SampleParticles::Initialize,
            .ShutDown = Screens::SampleParticles::ShutDown,
            .Render = Screens::SampleParticles::Render
        },
		ScreenDefinition{
            .Id = Screen::SampleSimpleMap,
            .Name = "SampleSimpleMap",
            .LandingLabel = "Simple\nMap",
            .Initialize = Screens::SampleSimpleMap::Initialize,
            .ShutDown = Screens::SampleSimpleMap::ShutDown,
            .Render = Screens::SampleSimpleMap::Render
        },
        ScreenDefinition{
            .Id = Screen::SampleNav,
            .Name = "SampleNav",
            .LandingLabel = "Navigation",
            .Initialize = Screens::SampleNav::Initialize,
            .ShutDown = Screens::SampleNav::ShutDown,
            .Render = Screens::SampleNav::Render
        },
        ScreenDefinition{
            .Id = Screen::SampleGiantMap,
            .Name = "SampleGiantMap",
            .LandingLabel = "Giant\nMap",
            .Initialize = Screens::SampleGiantMap::Initialize,
            .ShutDown = Screens::SampleGiantMap::ShutDown,
            .Render = Screens::SampleGiantMap::Render
        },
        ScreenDefinition{
            .Id = Screen::SampleCombat,
            .Name = "SampleCombat",
            .LandingLabel = "Combat",
            .Initialize = Screens::SampleCombat::Initialize,
            .ShutDown = Screens::SampleCombat::ShutDown,
            .Render = Screens::SampleCombat::Render
        }
    };

    auto activeScreenName = SampleUI::Screen::Landing;
	void (*activeScreenFn)() = SampleUI::Screens::Landing::Render;
} // namespace

namespace SampleUI {

    [[nodiscard]] std::span<const ScreenDefinition> GetScreenDefinitions() { return Definitions; }
    [[nodiscard]] const ScreenDefinition& GetScreenDefinition(Screen screen) {
        for(const auto& def: Definitions) {
            if(def.Id == screen) return def;
        }
        DR_ASSERT_MSG(false, "Invalid screen");
        return Definitions.front();
    }

	namespace Screens {
		bool Initialize() {
            SetActiveScreen(Screen::Landing);
            bool success = ::Ui::Dialog::Initialize();
            for(const auto& def : Definitions) {
                success &= !def.Initialize || def.Initialize();
            }
            return success;
		}

		void ShutDown() {
            for(auto& def: Definitions) {
                if(def.ShutDown) def.ShutDown();
            }
			::Ui::Dialog::ShutDown();
		}

		void Render() {
			activeScreenFn();
			::Ui::Dialog::Render();
			::Ui::Overlay::DrawFps();
		}

		void SetActiveScreen(Screen id) {
            auto& screen = GetScreenDefinition(id);
            activeScreenName = id;
            activeScreenFn = screen.Render;
		}

		Screen GetActiveScreen() {
			return activeScreenName;
		}
	} // namespace Screens
} // namespace SampleUI::Ui
