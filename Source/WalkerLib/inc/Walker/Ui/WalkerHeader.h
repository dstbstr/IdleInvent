#pragma once

namespace Walker::WalkerUi::Header {
    bool Initialize();
    void Render();
    void ShutDown();

    f32 GetRequestedHeight(f32 availableWidth);
}