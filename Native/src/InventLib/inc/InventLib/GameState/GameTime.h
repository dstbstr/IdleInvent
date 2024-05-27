#pragma once

#include "Core/NumTypes.h"

#include <chrono>

namespace Invent {
	using BaseTime = std::chrono::milliseconds;

	static inline constexpr BaseTime OneSecond = std::chrono::milliseconds(1);
	static inline constexpr BaseTime OneMinute = OneSecond * 60;
	static inline constexpr BaseTime FiveMinutes = OneMinute * 5;
	static inline constexpr BaseTime ThirtyMinutes = OneMinute * 30;
	static inline constexpr BaseTime OneHour = OneMinute * 60;
	static inline constexpr BaseTime FourHours = OneHour * 4;
	static inline constexpr BaseTime TwelveHours = OneHour * 12;
	static inline constexpr BaseTime OneDay = OneHour * 24;
    static inline constexpr BaseTime OneWeek = OneDay * 7;
    static inline constexpr BaseTime OneYear = OneDay * 365;
}