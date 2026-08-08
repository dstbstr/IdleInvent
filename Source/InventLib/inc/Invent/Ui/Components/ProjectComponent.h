#pragma once

#include "Settings/PurchaseAmount.h"

namespace Invent {
    struct Project;
    struct Life;
}

namespace Ui::Components::Project {
    void RenderPlusMinus(size_t& available, size_t& spent, PurchaseAmount purchaseAmount);
    void Render(Invent::Life& life, Invent::Project& project, PurchaseAmount purchaseAmount);
} // namespace Ui::Components::Project