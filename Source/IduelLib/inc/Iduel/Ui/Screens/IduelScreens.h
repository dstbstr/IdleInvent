#pragma once
#include "Platform/NumTypes.h"
#include "Ui/ScreenManager.h"

#include <string>

namespace Iduel::IduelUi {
    enum struct Screen : u8 { Start, Match, Review, COUNT };

    Ui::ScreenManager<Screen>& GetScreenManager();
} // namespace Iduel::IduelUi
