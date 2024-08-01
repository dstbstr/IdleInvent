#pragma once

#include "InventLib/Inventory/Item.h"

#include <vector>
#include <unordered_map>

namespace Invent {
    struct Inventory {
        std::unordered_map<ItemType, size_t> Equipment{};
        std::vector<size_t> Pack;
    };
}