#include "InventLib/Character/Life.h"

namespace Invent {
    void Life::Start(ResourceName specialty) {
        Specialty = specialty;  
    }

    // Update bonuses based on max research level
    void Life::Retire() {}

    void Life::Tick(BaseTime elapsed) {
        auto& primary = Resources.at(ResourceName::Primary);
        auto progress = WorkProgress.GetProgress(elapsed);
        primary.Current += progress;
        primary.Clamp();
    }
} // namespace Invent