#pragma once

#include <Platform/NumTypes.h>

using Handle = u64;
constexpr Handle InvalidHandle = static_cast<Handle>(-1);

namespace Handles {
    Handle Next();
}
