#include "InventLib/Mechanics/Modifier.h"

#include <format>

namespace Invent {
    void Modifier::Save(ModifierSave& outSave) const {
        outSave.Add = static_cast<u16>(Add);
        outSave.Mul = static_cast<u16>(Mul * 1000.f);
    }

    void ModifierSave::Load(Modifier& outModifier) const {
		outModifier.Add = Add;
		outModifier.Mul = Mul / 1000.f;
	}

    std::string ToString(const Modifier& mod) {
        return std::format("Add: {}, Mul: {}", mod.Add, mod.Mul);
	}

} // namespace Invent