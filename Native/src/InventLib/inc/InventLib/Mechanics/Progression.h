#pragma once

#include "InventLib/GameState/GameTime.h"

#include "Core/NumTypes.h"
#include "Core/Instrumentation/Logging.h"

#include <vector>
#include <numeric>

namespace Invent {
    struct ModifierSave {
        u8 DurationInYears{};
        u8 Add{0};
        u16 Mul{0}; // fixed point, max value = 65535 = 65.535
    };

    struct ProgressionSave {
        ModifierSave Permanent{};
        ModifierSave Temporary[8]{}; // yuck
        u8 TempCount : 3 {0};
    };

	struct Modifier {
        s64 Add{0};
        f32 Mul{1.0f};
        BaseTime Duration{OneYear * 100};
        void Save(ModifierSave& save) const;

        constexpr Modifier operator+(const Modifier& other) const {
			return {Add + other.Add, Mul * other.Mul, Duration};
		}
        constexpr f32 GetBonus() const { return Add * Mul; }
    };

	struct Progression {
        Progression() = default;
        Progression(const ProgressionSave& save);
        Progression(const Progression& other);
        Progression& operator=(const Progression & other);

        void Save(ProgressionSave& save) const;

        void AddModifier(Modifier mod);
        void ClearModifiers();

		s64 GetProgress(BaseTime elapsed);

	private:
        void CalcProgress();

		f32 m_Remainder{ 0.0f };
        Modifier m_Permanent {1, 1.0F};
        std::vector<Modifier> m_Modifiers{};
        f32 m_Progress{1.0f};
	};
}