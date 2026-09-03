	#include "Pets/Pets/PetDetails.h"

#include <map>

namespace {
	std::map<Pets::PetKind, Pets::PetDetails>& GetDetailMap(){
        static std::map<Pets::PetKind, Pets::PetDetails> DetailMap{};
		return DetailMap;
    }

	#define PET(_Kind, _L1, _Attack, _L2, _Piercing, _L3, _Capacity) \
		namespace _Kind##Registry{ \
			constexpr auto Kind = Pets::PetKind::_Kind; \
			void Register(); \
			struct Registrar { \
				Registrar() { \
					GetDetailMap()[Kind] = Pets::PetDetails{ \
						.Kind = Kind, \
						.Attack = _Attack, \
						.Piercing = _Piercing, \
						.PetCapacity = _Capacity \
					}; \
					Register(); \
				} \
			} registrar; \
        } \
		void _Kind##Registry::Register()

	#define LEVEL(_Level, _Target, _Kind, _Effect, _Value) \
		GetDetailMap()[Kind].Abilities.push_back({ \
			.Level = _Level, \
			.Value = { \
				.Kind = Pets::AbilityKind::_Kind, \
				.Target = Pets::AbilityTarget::_Target, \
				.Effect = Pets::AbilityEffect::_Effect, \
				.Value = _Value \
			} \
		});

	PET(Hero, Atk, 5, Pierce, 0, Cap, 1) {}
	PET(Bloodhound, Atk, 5, Pierce, 2, Cap, 1) {
		LEVEL(3, Self, Attack, Add, 5);
		LEVEL(5, Pets, Capacity, Multiply, 0.f);
	}
	PET(Labrador, Atk, 2, Pierce, 0, Cap, 2) {
		LEVEL(3, Siblings, Capacity, Add, 1.f);
	}
	PET(Poodle, Atk, 3, Pierce, 5, Cap, 1) {
		LEVEL(2, Self, Attack, Multiply, 2.f);
	}

#undef PET
#undef LEVEL
}

namespace Pets::Details {

	[[nodiscard]] const PetDetails& Get(PetKind kind) {
		return GetDetailMap().at(kind);
	}
}