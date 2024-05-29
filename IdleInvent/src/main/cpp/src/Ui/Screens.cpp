#include "Ui/Screens.h"
#include "Ui/Screens/Achievements.h"
#include "Ui/Screens/Purchases.h"
#include "Ui/Screens/Resources.h"
#include "Ui/Screens/Splash.h"
#include "Ui/Screens/Storages.h"
#include "Ui/Screens/Settings.h"

namespace {
    auto activeScreen = Ui::Screen::Splash;
}

namespace Ui::Screens {

    bool InitializeScreens() {
        return 
            Resources::Initialize() &&
            Storages::Initialize() &&
            Achievements::Initialize() &&
            Purchases::Initialize() &&
            Settings::Initialize();
    }

    void RenderActiveScreen() {
        switch (activeScreen) {
            case Screen::Splash:
                Splash::Render();
                break;
            case Screen::Resources:
                Resources::Render();
                break;
            case Screen::Storages:
                Storages::Render();
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