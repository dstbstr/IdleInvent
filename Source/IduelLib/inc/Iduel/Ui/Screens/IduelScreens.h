#pragma once
#include "Platform/NumTypes.h"
#include "Ui/ScreenManager.h"

#include <string>

namespace Iduel::IduelUi {
    enum struct Screen : u8 { Start, Match, Review, Editor, COUNT };

    Ui::ScreenManager<Screen>& GetScreenManager();

    namespace Screens {
        namespace Match {
            bool Initialize();
            void ShutDown();
            void Render();
        }
        namespace Start {
            bool Initialize();
            void ShutDown();
            void Render();
        }
        namespace Review {
            bool Initialize();
            void ShutDown();
            void Render();
        }
        namespace Editor {
            bool Initialize();
            void ShutDown();
            void Render();
        }
    }
} // namespace Iduel::IduelUi
