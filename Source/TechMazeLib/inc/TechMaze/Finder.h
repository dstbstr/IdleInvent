#pragma once
#include "Platform/NumTypes.h"

#include <string>
#include <array>

namespace TechMaze {
    enum struct EraStatus : u8 { Unknown, Low, High, Correct, LowLevel};
    enum struct DetailLevel : u8 { CountCorrect, CountLowHigh, LowHighDetail, AllDetail};
	struct FindResult {
        std::array<EraStatus, 5> Status{};
        u8 CountCorrect{0};
        u8 CountLow{0};
        u8 CountHigh{0};
	};
	struct Finder {
        FindResult Find(u64 currentPos, u64 targetPos) const;

        DetailLevel Level{0};
	};
} // namespace TechMaze