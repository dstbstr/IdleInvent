#pragma once
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Tools/Tools.h"

#include <vector>

namespace GhostHunter::Inventory {
    std::vector<Media>& GetMedia();
    void SellAllMedia();
    void SellMedia(size_t index);

    std::vector<Tool>& GetTools();

    void Initialize();
    void ShutDown();
}