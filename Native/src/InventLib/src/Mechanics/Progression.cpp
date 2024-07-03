#include "InventLib/Mechanics/Progression.h"

namespace Invent {
    void Modifier::Save(ModifierSave& save) const {
		save.Add = static_cast<u8>(Add);
		save.Mul = static_cast<u16>(Mul * 1000.f);
		save.DurationInYears = static_cast<u8>(Duration / OneGameYear);
	}

    Progression::Progression(const Progression& other) {
        m_Modifiers = other.m_Modifiers;
        CalcProgress();
    }

    Progression::Progression(const ProgressionSave& save) {
        m_Permanent = {std::max(u8(1), save.Permanent.Add), std::max(1.0f, save.Permanent.Mul / 1000.f), Forever};

        for(int i = 0; i < save.TempCount; i++) {
            const auto& mod = save.Temporary[i];
            m_Modifiers.push_back({mod.Add, mod.Mul / 1000.f, mod.DurationInYears * OneGameYear});
        }

        CalcProgress();

        Log::Info(std::format("Loaded Progression Progress={}", m_Progress));
    }

    void Progression::Save(ProgressionSave& save) const {
        m_Permanent.Save(save.Permanent);
        save.TempCount = static_cast<u8>(m_Modifiers.size());
        for(auto i = 0u; i < m_Modifiers.size(); i++) {
			const auto& mod = m_Modifiers[i];
			mod.Save(save.Temporary[i]);
		}
	}

    Progression& Progression::operator=(const Progression& other) {
		m_Modifiers = other.m_Modifiers;
		CalcProgress();
		return *this;
	}

    void Progression::AddModifier(Modifier mod) {
        if (mod.Duration == Forever) {
            m_Permanent.Add += mod.Add;
            m_Permanent.Mul *= mod.Mul;
        } else {
            m_Modifiers.push_back(mod);
        }

        CalcProgress();
    }

    void Progression::ClearModifiers() {
        m_Modifiers.clear();
        m_Progress = 0.F;
    }

    s64 Progression::GetProgress(BaseTime elapsed) {
        bool recalc = false;
        for(int i = static_cast<int>(m_Modifiers.size() - 1); i >= 0; i--) {
            auto& mod = m_Modifiers[i];
            mod.Duration -= elapsed;
            if(mod.Duration.count() <= 0) {
                std::swap(m_Modifiers[i], m_Modifiers.back());
                m_Modifiers.pop_back();
                recalc = true;
            }
        }

        if(recalc) {
            CalcProgress();
        }

        auto seconds = static_cast<f32>(elapsed.count()) / static_cast<f32>(OneSecond.count());
        auto progress = (m_Progress * seconds) + m_Remainder;

        m_Remainder = progress - static_cast<s64>(progress);
        return static_cast<s64>(progress);
    }

    void Progression::CalcProgress() {
        auto mod = std::accumulate(
            m_Modifiers.begin(),
            m_Modifiers.end(),
            m_Permanent,
            [](const Modifier& running, const Modifier& next) { return running + next; }
        );

        m_Progress = mod.GetBonus();
    }
}