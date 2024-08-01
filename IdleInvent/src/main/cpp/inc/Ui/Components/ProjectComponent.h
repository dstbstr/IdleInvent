#pragma once

#include "InventLib/Settings/PurchaseAmount.h"

namespace Invent {
    struct Project;
    struct Life;
}

namespace Ui::Components::Project {
    enum struct StartCondition { None, RequireResources};
    void Render(Invent::Life& life, Invent::Project& project, Invent::PurchaseAmount purchaseAmount, StartCondition startCondition = StartCondition::None);
} // namespace Ui::Components::Project