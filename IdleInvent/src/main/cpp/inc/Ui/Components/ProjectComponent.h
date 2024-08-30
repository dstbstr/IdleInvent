#pragma once

#include "InventLib/Settings/PurchaseAmount.h"

namespace Invent {
    struct Project;
    struct Life;
}

namespace Ui::Components::Project {
    void RenderPlusMinus(size_t& available, size_t& spent, Invent::PurchaseAmount purchaseAmount);
    void Render(Invent::Life& life, Invent::Project& project, Invent::PurchaseAmount purchaseAmount);
} // namespace Ui::Components::Project