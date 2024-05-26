#pragma once

#include "Core/NumTypes.h"

namespace Invent {
	static inline constexpr size_t OneSecond = 1;
	static inline constexpr size_t OneMinute = OneSecond * 60;
	static inline constexpr size_t FiveMinutes = OneMinute * 5;
	static inline constexpr size_t ThirtyMinutes = OneMinute * 30;
	static inline constexpr size_t OneHour = OneMinute * 60;
	static inline constexpr size_t FourHours = OneHour * 4;
	static inline constexpr size_t TwelveHours = OneHour * 12;
	static inline constexpr size_t OneDay = OneHour * 24;
}