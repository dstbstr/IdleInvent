#pragma once

namespace Ui {
    enum struct Screen { Splash, Home, Achievements, Purchases, Settings };

    namespace Screens {
        bool InitializeScreens();
        void ShutDownScreens();
        
        void RenderActiveScreen();
        void SetActiveScreen(Screen screen);
        Screen GetActiveScreen();
    } // namespace Screens
} // namespace Ui