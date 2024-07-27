#pragma once

#include "InventLib/Mechanics/Effect.h"

#include <string>
#include <unordered_map>

namespace Invent {
    std::unordered_map<std::string, Effect> GetAllRebirthBenefits();
}