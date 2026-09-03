#pragma once

#include <Platform/NumTypes.h>

namespace Pets {
	enum struct AbilityKind : u8 {
		Ability,
		Capacity,
		Attack,
		Piercing,
		Mana,
		ActionRate
	};

	enum struct AbilityTarget : u8 {
		Hero,
		Self,
		Owner,
		Pets,
		Siblings
	};

	enum struct AbilityEffect : u8 {
		Add,
		Multiply
	};

	struct Ability {
		AbilityKind Kind{};
		AbilityTarget Target{};
		AbilityEffect Effect{};
		f32 Value{};
	};
}