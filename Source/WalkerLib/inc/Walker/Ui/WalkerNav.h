#pragma once

namespace Walker::WalkerUi::Nav {
    bool Initialize();
    void Render();
    void ShutDown();

    f32 GetRequestedHeight(f32 availableWidth);
} // namespace Walker::WalkerUi::Nav