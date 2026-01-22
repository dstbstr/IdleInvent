#pragma once
#include <string>

namespace GhostHunter::Ui {
    enum struct Screen { Editing, Investigate, Market, Prestige, Settings };

    std::string ToString(Screen screen);

    namespace Screens {
        bool Initialize();
        void ShutDown();

        void Render();
        void SetActiveScreen(Screen screen);
        Screen GetActiveScreen();
    } // namespace Screens
} // namespace Ui