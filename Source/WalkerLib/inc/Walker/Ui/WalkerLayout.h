#pragma once
#include <GameState/GameTime.h>

namespace Walker::WalkerUi::Layout {
    bool Initialize();
    void Render();
    void ShutDown();
    void Tick(BaseTime elapsed);
} // namespace Walker::WalkerUi::Layout