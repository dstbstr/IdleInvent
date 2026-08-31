#pragma once

// Standard library headers that are heavily used across the project
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <unordered_set>
#include <memory>
#include <optional>
#include <variant>
#include <functional>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <concepts>
#include <initializer_list>
#include <utility>
#include <tuple>
#include <format>
#include <filesystem>
#include <system_error>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <array>

// Platform and Engine headers
#include <Platform/NumTypes.h>
#include <Platform/Platform.h>
#include <GameState/GameTime.h>
#include <Resources/Resource.h>
#include <Instrumentation/Logging.h>
#include <Constexpr/ConstexprStrUtils.h>

// Frequently used InventLib headers
#include "Invent/Resources/InventResources.h"
#include "Invent/Projects/Project.h"
#include "Invent/Character/Life.h"
#include "Invent/GameState/GameSettings.h"
