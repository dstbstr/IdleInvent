#include "GhostHunter/Ui/Screens/Screens.h"

#include "GhostHunter/Ui/Screens/JournalScreen.h"
#include "GhostHunter/Ui/Screens/LocationScreen.h"
#include "GhostHunter/Ui/Screens/MediaScreen.h"
#include "GhostHunter/Ui/Screens/PrestigeScreen.h"
#include "GhostHunter/Ui/Screens/ToolsScreen.h"
#include "GhostHunter/Ui/Screens/SettingsScreen.h"

#include <functional>
#include <Instrumentation/Logging.h>

namespace {
    auto activeScreenName = GhostHunter::Ui::Screen::Media;
    void (*activeScreenFn)() = GhostHunter::Ui::Screens::Media::Render;
} // namespace

namespace GhostHunter::Ui {
    std::string ToString(Screen screen) {
        switch(screen) {
            using enum Screen;
            case Journal: return "Journal";
            case Location: return "Location";
            case Media: return "Media";
            case Prestige: return "Prestige";
            case Tools: return "Tools";
            case Settings: return "Settings";
        }

        DR_ASSERT_MSG(false, "Invalid screen");
        return "Unknown screen";
    }

    namespace Screens {

        bool Initialize() {
            return 
                Journal::Initialize() && 
                Location::Initialize() && 
                Media::Initialize() && 
                Prestige::Initialize() &&
                Settings::Initialize() && 
                Tools::Initialize();
        }

        void ShutDown() {
                Journal::ShutDown();
                Location::ShutDown();
                Media::ShutDown();
                Prestige::ShutDown();
                Settings::ShutDown();
                Tools::ShutDown();
            activeScreenName = Screen::Media;
            activeScreenFn = {};
        }

        void Render() { activeScreenFn(); }

        void SetActiveScreen(Screen screen) {
            activeScreenName = screen;
            switch(screen) {
                using enum Screen;
                case Journal: activeScreenFn = Journal::Render; break;
                case Location: activeScreenFn = Location::Render; break;
                case Media: activeScreenFn = Media::Render; break;
                case Prestige: activeScreenFn = Prestige::Render; break;
                case Tools: activeScreenFn = Tools::Render; break;
                case Settings: activeScreenFn = Settings::Render; break;
            }
        }

        Screen GetActiveScreen() { return activeScreenName; }
    } // namespace Screens
} // namespace Ui