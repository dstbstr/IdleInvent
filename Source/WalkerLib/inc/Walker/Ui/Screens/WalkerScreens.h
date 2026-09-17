#pragma once
#include "Platform/NumTypes.h"

#include <string>

namespace Walker::WalkerUi {
    enum struct Screen : u8 { Travel, Crew, Rebirth, Settings };

    std::string ToString(Screen screen);

    namespace Screens {
        bool Initialize();
        void ShutDown();

        void Render();
        void SetActiveScreen(Screen screen);
        Screen GetActiveScreen();
    } // namespace Screens
} // namespace Walker::WalkerUi
