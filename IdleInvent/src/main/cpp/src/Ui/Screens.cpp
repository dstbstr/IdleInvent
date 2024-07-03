#include "Ui/Screens.h"
#include "Ui/Screens/Achievements.h"
#include "Ui/Screens/Purchases.h"
#include "Ui/Screens/Home.h"
#include "Ui/Screens/Splash.h"
#include "Ui/Screens/Settings.h"

namespace {
    auto activeScreen = Ui::Screen::Splash;
}

namespace Ui::Screens {

    bool InitializeScreens() {
        return
            Home::Initialize() &&
            Achievements::Initialize() &&
            Purchases::Initialize() &&
            Settings::Initialize();
    }

    void ShutDownScreens() {
        Home::ShutDown();
        Achievements::ShutDown();
        Purchases::ShutDown();
        Settings::ShutDown();
        activeScreen = Screen::Splash;
    }

    void RenderActiveScreen() {
        switch (activeScreen) {
            case Screen::Splash:
                Splash::Render();
                break;
            case Screen::Home:
                Home::Render();
                break;
            case Screen::Achievements:
                Achievements::Render();
                break;
            case Screen::Purchases:
                Purchases::Render();
                break;
            case Screen::Settings:
                Settings::Render();
                break;
        }
    }

    void SetActiveScreen(Screen screen) {
        activeScreen = screen;
    }

    Screen GetActiveScreen() {
        return activeScreen;
    }
}