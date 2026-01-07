#pragma once

#include "GameState/GameTime.h"

namespace GhostHunter::Market {
    void Init();
    void ShutDown();

    void Update(BaseTime elapsed);
}