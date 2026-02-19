#include "GhostHunter/Media/Media.h"

#include "GhostHunter/Resources/GhostHunterResources.h"
#include "Mechanics/Purchasable.h"
#include "Mechanics/Upgrade.h"

namespace GhostHunter {
    static_assert(UpgradableType<Media>, "Expected Media to be upgradable");

    std::string Media::Describe() const {
        return "Something here";
    }

    void Media::OnUpgrade() {
    }
} // namespace GhostHunter