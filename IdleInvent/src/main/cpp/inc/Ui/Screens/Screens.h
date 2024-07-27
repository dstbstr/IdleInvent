#pragma once

#include <string>

namespace Ui {
    enum struct Screen { StartLife, Home, Achievements, Purchases, Settings, Build, Explore, Research };

    std::string ToString(Screen screen);

    namespace Screens {
        bool Initialize();
        void ShutDown();
        
        void Render();
        void SetActiveScreen(Screen screen);
        Screen GetActiveScreen();
    } // namespace Screens
} // namespace Ui