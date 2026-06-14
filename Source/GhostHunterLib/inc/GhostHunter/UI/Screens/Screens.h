#pragma once
#include <string>
#include "Platform/NumTypes.h"

namespace GhostHunter::Ui {
    enum struct Screen : u8 { Editing, Investigate, Market, Prestige, Settings };

    std::string ToString(Screen screen);

    namespace Screens {
        bool Initialize();
        void ShutDown();

        void Render();
        void SetActiveScreen(Screen screen);
        Screen GetActiveScreen();
    } // namespace Screens
} // namespace Ui