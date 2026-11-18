#pragma once

#include "Platform/NumTypes.h"

#include <string_view>

namespace Math {
	f64 Eval(std::string_view expr, f64 x = 0.0);
}