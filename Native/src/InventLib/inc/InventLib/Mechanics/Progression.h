#pragma once

#include "InventLib/GameState/GameTime.h"

#include "InventLib/Mechanics/Modifier.h"

#include "Core/NumTypes.h"
#include "Core/Instrumentation/Logging.h"

#include <vector>
#include <numeric>

namespace Invent {
    struct ProgressionSave {
        ModifierSave Permanent{};
        ModifierSave Temp[8]{}; //yuck
        u8 Durations[8]{}; //yuck++
        u8 TempCount{0};
    };

	struct Progression {
        Progression() = default;
        Progression(const ProgressionSave& save);
        Progression(const Progression& other);
        Progression& operator=(const Progression & other);

        void Save(ProgressionSave& save) const;

        void AddPermanent(Modifier mod);
        void AddTemp(Modifier mod, BaseTime duration);

        void ClearModifiers();

		s64 GetProgress(BaseTime elapsed);

	private:
        void CalcProgress();

		f32 m_Remainder{ 0.0f };
        Modifier m_Permanent {0, 1.0F};
        std::vector<std::pair<Modifier, BaseTime>> m_Modifiers{};
        f32 m_Progress{0.0f};
	};
}