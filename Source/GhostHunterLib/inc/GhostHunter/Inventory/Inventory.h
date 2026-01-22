#pragma once
#include "GhostHunter/Investigation/Evidence.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Tools/Tools.h"

#include "Resources/Resource.h"

#include <vector>

namespace GhostHunter {
    struct Inventory {
        ResourceCollection Resources;
        std::vector<Evidence> CollectedEvidence;
        std::vector<Media> CreatedMedia;
        std::vector<Tool> OwnedTools;
    };
}