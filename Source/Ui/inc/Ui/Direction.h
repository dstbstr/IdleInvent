#pragma once

#include <Constexpr/ConstexprMath.h>
#include <Platform/NumTypes.h>

#include <imgui.h>

#include <cmath>

namespace Ui {
	// 8 cardinal directions in 2D screen space. North is "up" (negative y), east is "right"
	// (positive x). Same as a compass overlay on the screen.
	enum struct Direction : u8 {
		N, NE, E, SE, S, SW, W, NW
	};

	// Angle in radians for each direction, screen-space convention (y-down).
	// North = -pi/2 (matches the "270 degrees == up" convention used elsewhere).
	[[nodiscard]] constexpr f32 ToAngleRadians(Direction d) {
		switch(d) {
			using enum Direction;
			case E:  return Constexpr::DegToRad(0.f);
			case SE: return Constexpr::DegToRad(45.f);
			case S:  return Constexpr::DegToRad(90.f);
			case SW: return Constexpr::DegToRad(135.f);
			case W:  return Constexpr::DegToRad(180.f);
			case NW: return Constexpr::DegToRad(-135.f);
			case N:  return Constexpr::DegToRad(-90.f);
			case NE: return Constexpr::DegToRad(-45.f);
		}
		return 0.f;
	}

	// Unit vector for the direction. Y is negated for N/NE/NW because screen-y points down.
	[[nodiscard]] inline ImVec2 ToVector(Direction d) {
		const f32 angle = ToAngleRadians(d);
		return ImVec2{std::cos(angle), std::sin(angle)};
	}
}
