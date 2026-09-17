#include "Walker/Ui/Screens/WalkerScreens.h"
#include "Walker/Ui/Screens/CrewScreen.h"
#include "Walker/Ui/Screens/RebirthScreen.h"
#include "Walker/Ui/Screens/SettingsScreen.h"
#include "Walker/Ui/Screens/TravelScreen.h"
#include <Instrumentation/Logging.h>

namespace {
    auto ActiveScreenName = Walker::WalkerUi::Screen::Travel;
    void (*ActiveScreenFn)() = Walker::WalkerUi::Screens::Travel::Render;
} // namespace

namespace Walker::WalkerUi {
    std::string ToString(Screen screen) {
        switch(screen) {
            using enum Screen;
            case Travel: return "Travel";
            case Crew: return "Crew";
            case Rebirth: return "Rebirth";
            case Settings: return "Settings";
        }

        DR_ASSERT_MSG(false, "Unknown screen");
        return "Unknown";
    }

    namespace Screens {
        bool Initialize() { 
            auto success = true;
            success &= Travel::Initialize();
            success &= Crew::Initialize();
            success &= Rebirth::Initialize();
            success &= Settings::Initialize();

            DR_ASSERT(success);
            return success;
        }

        void ShutDown() {
            Settings::ShutDown();
            Rebirth::ShutDown();
            Crew::ShutDown();
            Travel::ShutDown();
        }

        void Render() { ActiveScreenFn(); }

        void SetActiveScreen(Screen screen) {
            ActiveScreenName = screen;
            switch(screen) {
                using enum Screen;
                case Travel: ActiveScreenFn = Travel::Render; break;
                case Crew: ActiveScreenFn = Crew::Render; break;
                case Rebirth: ActiveScreenFn = Rebirth::Render; break;
                case Settings: ActiveScreenFn = Settings::Render; break;
            }
        }

        Screen GetActiveScreen() { 
            return ActiveScreenName;
        }
    } // namespace Screens
} // namespace Walker::WalkerUi
