#pragma once

namespace Ui {
    enum struct Screen { Splash, Resources, Storages, Achievements, Purchases, Settings };

    namespace Screens {
        bool InitializeScreens();
        void RenderActiveScreen();
        void SetActiveScreen(Screen screen);
        Screen GetActiveScreen();
    } // namespace Screens
} // namespace Ui