#include "Pets/Ui/Screens/Screens.h"

#include "Pets/Ui/Screens/PetsScreen.h"
#include "Pets/Ui/Screens/CombatScreen.h"
#include "Pets/Ui/Screens/BestiaryScreen.h"
#include "Pets/Ui/Screens/RebirthScreen.h"

#include <Instrumentation/Logging.h>

namespace {
	auto activeScreenName = Pets::Ui::Screen::Pets;
	void (*activeScreenFn)() = Pets::Ui::Screens::Pets::Render;
} // namespace

namespace Pets::Ui {
	std::string ToString(Screen screen) {
		switch(screen) {
			using enum Screen;
			case Pets: return "Pets";
			case Combat: return "Combat";
			case Bestiary: return "Bestiary";
			case Rebirth: return "Rebirth";
            case Settings: return "Settings";
		}

		DR_ASSERT_MSG(false, "Invalid screen");
		return "Unknown screen";
	}

	namespace Screens {

		bool Initialize() {
			return
				Pets::Initialize() &&
				Combat::Initialize() &&
				Bestiary::Initialize() &&
				Rebirth::Initialize();
		}

		void ShutDown() {
			Pets::ShutDown();
			Combat::ShutDown();
			Bestiary::ShutDown();
			Rebirth::ShutDown();
			activeScreenName = Screen::Pets;
			activeScreenFn = {};
		}

		void Render() { activeScreenFn(); }

		void SetActiveScreen(Screen screen) {
			activeScreenName = screen;
			switch(screen) {
				using enum Screen;
				case Pets: activeScreenFn = Pets::Render; break;
				case Combat: activeScreenFn = Combat::Render; break;
				case Bestiary: activeScreenFn = Bestiary::Render; break;
				case Rebirth: activeScreenFn = Rebirth::Render; break;
                default: break;
			}
		}

		Screen GetActiveScreen() { return activeScreenName; }
	} // namespace Screens
} // namespace Pets::Ui
