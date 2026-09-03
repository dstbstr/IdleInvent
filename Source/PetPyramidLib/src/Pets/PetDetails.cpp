#include "Pets/Pets/PetDetails.h"

#include <map>

namespace {
	std::map<Pets::PetKind, Pets::PetDetails> DetailMap{};
}

namespace Pets::Details {

	[[nodiscard]] const PetDetails& Get(PetKind kind) {
		return DetailMap.at(kind);
	}
}