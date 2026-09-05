#include "Pets/Character/PartyEditor.h"

namespace {
    const Pets::ResolvedPet* FindResolution(const Pets::PartyResolution& resolution, const Pets::PartyNode& node) {
        for(const auto& pet : resolution.Pets) {
            if(pet.Node == &node) {
                return &pet;
            }
        }
        return nullptr;
    }

    bool AddablePetKind(Pets::PetKind kind) {
        return kind > Pets::PetKind::Hero && kind < Pets::PetKind::COUNT;
    }
}

namespace Pets {
    PartyEditor::PartyEditor(Party& party, const PetRoster& roster) 
        : m_Party(party)
        , m_Roster(roster)
        , m_Resolution(PartyResolver::Resolve(m_Party, m_Roster))
    {
    }

    const PartyResolution& PartyEditor::GetResolution() const {
        return m_Resolution;    
    }

    const ResolvedPet* PartyEditor::GetResolvedPet(const PartyNode& node) const {
        return FindResolution(m_Resolution, node);
    }
    
    bool PartyEditor::CanAddPet(const PartyNode& parent, PetKind kind) const {
        // don't add heros
        if(!AddablePetKind(kind)) return false;
        // can't add something you don't own
        if(!m_Roster[kind].has_value()) return false;
        
        // no duplicates
        for(const auto& resolvedPet : m_Resolution.Pets) {
            if(resolvedPet.Kind == kind) return false;
        }

        const auto* resolvedParent = FindResolution(m_Resolution, parent);

        // Parent must belong to the party and not at max depth
        if(!resolvedParent || resolvedParent->Depth >= m_Party.MaxDepth) return false;
        
        // check parent's capacity
        return parent.Pets.size() < resolvedParent->PetCapacity;
    }
    
    bool PartyEditor::AddPet(PartyNode& parent, PetKind kind) {
        if(!CanAddPet(parent, kind)) return false;

        parent.Pets.push_back(PartyNode{.Kind = kind});
        m_Resolution = PartyResolver::Resolve(m_Party, m_Roster);

        return true;
    }
    
    bool PartyEditor::RemovePet(PartyNode& parent, size_t index) {
        if(index >= parent.Pets.size()) return false;

        parent.Pets.erase(parent.Pets.begin() + index);
        m_Resolution = PartyResolver::Resolve(m_Party, m_Roster);

        return true;
    }
}