#include "InventLib/Mechanics/Progression.h"

namespace Invent {
    Progression::Progression(const Progression& other) {
        m_Permanent = other.m_Permanent;
        m_Modifiers = other.m_Modifiers;
        CalcProgress();
    }

    Progression::Progression(const ProgressionSave& save) {
        save.Permanent.Load(m_Permanent);
        for(auto i = 0u; i < save.TempCount; i++) {
			Modifier mod;
			save.Temp[i].Load(mod);
            m_Modifiers.push_back({mod, OneMinute * save.Durations[i]});
		}

        CalcProgress();

        Log::Info(std::format("Loaded Progression Progress={}", m_Progress));
    }

    void Progression::Save(ProgressionSave& save) const {
        m_Permanent.Save(save.Permanent);
        
        save.TempCount = static_cast<u8>(m_Modifiers.size());
        for(auto i = 0u; i < m_Modifiers.size(); i++) {
			const auto& [mod, duration] = m_Modifiers[i];
			mod.Save(save.Temp[i]);
            save.Durations[i] = static_cast<u8>(duration.count() / OneMinute.count());
		}
	}

    Progression& Progression::operator=(const Progression& other) {
        m_Permanent = other.m_Permanent;
		m_Modifiers = other.m_Modifiers;
		CalcProgress();
		return *this;
	}

    void Progression::AddPermanent(Modifier mod) {
        m_Permanent += mod;

        CalcProgress();
    }

    void Progression::AddTemp(Modifier mod, BaseTime duration) {
		m_Modifiers.push_back({mod, duration});
		CalcProgress();
	}

    void Progression::ClearModifiers() {
        m_Modifiers.clear();
        m_Progress = 0.F;
    }

    s64 Progression::GetProgress(BaseTime elapsed) {
        bool recalc = false;
        for(int i = static_cast<int>(m_Modifiers.size() - 1); i >= 0; i--) {
            auto& [mod, duration] = m_Modifiers[i];
            duration -= elapsed;
            if(duration.count() <= 0) {
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
        auto mod = m_Permanent;
        for(const auto& [modifier, _]: m_Modifiers) {
			mod += modifier;
		}

        m_Progress = mod.GetBonus();
    }
}