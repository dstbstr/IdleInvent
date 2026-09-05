#pragma once

#include "Pets/Character/Party.h"
#include "Pets/Character/PartyResolver.h"

namespace Pets {
	class PartyEditor {
    public:
		PartyEditor(Party& party, const PetRoster& roster);

		const PartyResolution& GetResolution() const;
		const ResolvedPet* GetResolvedPet(const PartyNode& node) const;

		bool CanAddPet(const PartyNode& parent, PetKind pet) const;
		bool AddPet(PartyNode& parent, PetKind kind);
        bool RemovePet(PartyNode& parent, size_t index);

	private:
		Party& m_Party;
		const PetRoster& m_Roster;
		PartyResolution m_Resolution{};
	};
}