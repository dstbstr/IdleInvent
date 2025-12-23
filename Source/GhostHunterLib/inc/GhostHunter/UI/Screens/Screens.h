#pragma once
#include <string>

namespace GhostHunter::Ui {
    enum struct Screen { Journal, Location, Media, Prestige, Tools, Settings };

    std::string ToString(Screen screen);

    namespace Screens {
        bool Initialize();
        void ShutDown();

        void Render();
        void SetActiveScreen(Screen screen);
        Screen GetActiveScreen();
    } // namespace Screens
} // namespace Ui