#include "Ui/Screens/Screens.h"

#include "Ui/Screens/Achievements.h"
#include "Ui/Screens/Build.h"
#include "Ui/Screens/Explore.h"
#include "Ui/Screens/Home.h"
#include "Ui/Screens/Purchases.h"
#include "Ui/Screens/Research.h"
#include "Ui/Screens/Settings.h"
#include "Ui/StartLife.h"

#include "Core/Instrumentation/Logging.h"

#include <functional>

namespace {
    auto activeScreenName = Ui::Screen::Home;
    void (*activeScreenFn)() = Ui::Screens::Home::Render;
} // namespace

namespace Ui {
    std::string ToString(Screen screen) {
        switch(screen) {
        case Screen::Home: return "Home";
        case Screen::Achievements: return "Achievements";
        case Screen::Purchases: return "Purchases";
        case Screen::Settings: return "Settings";
        case Screen::Build: return "Build";
        case Screen::Explore: return "Explore";
        case Screen::Research: return "Research";
        case Screen::StartLife: return "Start Life";
        }

        DR_ASSERT_MSG(false, "Invalid screen");
        return "Unknown screen";
    }

    namespace Screens {

        bool Initialize() {
            return Achievements::Initialize() && 
            Build::Initialize() &&
            Explore::Initialize() &&
            Home::Initialize() && 
            Purchases::Initialize() && 
            Research::Initialize() &&
            Settings::Initialize() && 
            StartLife::Initialize();
        }

        void ShutDown() {
            Achievements::ShutDown();
            Build::ShutDown();
            Explore::ShutDown();
            Home::ShutDown();
            Purchases::ShutDown();
            Research::ShutDown();
            Settings::ShutDown();
            StartLife::ShutDown();
            activeScreenName = Screen::Home;
            activeScreenFn = {};
        }

        void Render() {
            activeScreenFn();
        }

        void SetActiveScreen(Screen screen) {
            activeScreenName = screen;
            switch(screen) {
            case Screen::Home: activeScreenFn = Home::Render; break;
            case Screen::Achievements: activeScreenFn = Achievements::Render; break;
            case Screen::Build: activeScreenFn = Build::Render; break;
            case Screen::Explore: activeScreenFn = Explore::Render; break;
            case Screen::Purchases: activeScreenFn = Purchases::Render; break;
            case Screen::Research: activeScreenFn = Research::Render; break;
            case Screen::Settings: activeScreenFn = Settings::Render; break;
            case Screen::StartLife: activeScreenFn = StartLife::Render; break;
            }
        }

        Screen GetActiveScreen() {
            return activeScreenName;
        }
    } // namespace Screens
} // namespace Ui