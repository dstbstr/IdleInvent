#pragma once

#include "Platform/NumTypes.h"

#include <expected>
#include <string_view>

namespace Math {
	f64 Eval(std::string_view expr, f64 x = 0.0);
	std::expected<f64, std::string> TryEval(std::string_view expr, f64 x = 0.0);
}