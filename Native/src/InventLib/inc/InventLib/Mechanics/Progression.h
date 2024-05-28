#pragma once

#include "InventLib/GameState/GameTime.h"

#include "Core/NumTypes.h"

#include <vector>
#include <numeric>

namespace Invent {

	struct Modifier {
        s64 Add{0};
        f32 Mul{1.0f};
        BaseTime Duration{OneYear * 100};
    };

	struct Progression {
        std::vector<Modifier> Modifiers{{1, 1.0F}};
        bool Active{false};

		constexpr s64 GetProgress(BaseTime elapsed) {
            if(!Active) {
                return 0;
            }
            
            Modifier bonuses = {};
            for(int i = static_cast<int>(Modifiers.size() - 1); i >= 0; i--) {
                auto& mod = Modifiers[i];
                mod.Duration -= elapsed;
                if(mod.Duration.count() <= 0) {
                    std::swap(Modifiers[i], Modifiers.back());
                    Modifiers.pop_back();
                    continue;
                }
                bonuses.Add += mod.Add;
                bonuses.Mul *= mod.Mul;
			}

			auto seconds = static_cast<f32>(elapsed.count()) / 1000.0f;
			auto progress = (bonuses.Add * bonuses.Mul * seconds) + m_Remainder;

			m_Remainder = progress - static_cast<s64>(progress);
			return static_cast<s64>(progress);
		}

	private:
		f32 m_Remainder{ 0.0f };
	};
}