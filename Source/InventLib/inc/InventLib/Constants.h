#pragma once
#include <Platform/NumTypes.h>

namespace Invent::Constants {
	namespace Life {
        inline constexpr u8 WorkerDensity{5}; // Max Workers = CurrentPopulation / WorkerDensity
        inline constexpr u64 MaxWorkers{3};
        inline constexpr u64 CurrentPopulation{15};
        inline constexpr u64 MaxPopulation{30};
        inline constexpr u64 CurrentTimeShards{0};
        inline constexpr u64 MaxTimeShards{1 * 60 * 60 * 4}; // 4 hours

	}
}