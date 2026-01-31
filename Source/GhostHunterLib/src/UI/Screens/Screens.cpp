#include "GhostHunter/Ui/Screens/Screens.h"

#include "GhostHunter/Ui/Screens/EditingScreen.h"
#include "GhostHunter/Ui/Screens/InvestigateScreen.h"
#include "GhostHunter/Ui/Screens/MarketScreen.h"
#include "GhostHunter/Ui/Screens/PrestigeScreen.h"
#include "GhostHunter/Ui/Screens/SettingsScreen.h"

#include <Instrumentation/Logging.h>

namespace {
    auto activeScreenName = GhostHunter::Ui::Screen::Market;
    void (*activeScreenFn)() = GhostHunter::Ui::Screens::Market::Render;
} // namespace

namespace GhostHunter::Ui {
    std::string ToString(Screen screen) {
        switch(screen) {
            using enum Screen;
            case Editing: return "Editing";
            case Investigate: return "Investigate";
            case Market: return "Market";
            case Prestige: return "Prestige";
            case Settings: return "Settings";
        }

        DR_ASSERT_MSG(false, "Invalid screen");
        return "Unknown screen";
    }

    namespace Screens {

        bool Initialize() {
            return 
                Editing::Initialize() && 
                Investigate::Initialize() && 
                Market::Initialize() && 
                Prestige::Initialize() &&
                Settings::Initialize();
        }

        void ShutDown() {
                Editing::ShutDown();
                Investigate::ShutDown();
                Market::ShutDown();
                Prestige::ShutDown();
                Settings::ShutDown();
            activeScreenName = Screen::Market;
            activeScreenFn = {};
        }

        void Render() { activeScreenFn(); }

        void SetActiveScreen(Screen screen) {
            activeScreenName = screen;
            switch(screen) {
                using enum Screen;
                case Editing: activeScreenFn = Editing::Render; break;
                case Investigate: activeScreenFn = Investigate::Render; break;
                case Market: activeScreenFn = Market::Render; break;
                case Prestige: activeScreenFn = Prestige::Render; break;
                case Settings: activeScreenFn = Settings::Render; break;
            }
        }

        Screen GetActiveScreen() { return activeScreenName; }
    } // namespace Screens
} // namespace Ui