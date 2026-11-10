#pragma once
#include <GameState/GameTime.h>

namespace Iduel::IduelUi::Layout {
    bool Initialize();
    void Render();
    void ShutDown();
    void Tick(BaseTime elapsed);
} // namespace Iduel::IduelUi::Layout