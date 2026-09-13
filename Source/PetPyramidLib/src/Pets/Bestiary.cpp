#include "Pets/Pets/Bestiary.h"

namespace {
    size_t GetIndex(Pets::PetKind kind) {
        auto index = static_cast<size_t>(kind);
        DR_ASSERT_MSG(index > 0 && index < static_cast<size_t>(Pets::PetKind::COUNT), "Invalid PetKind");
        return index;
    }
}

namespace Pets {
    Bestiary::Bestiary() {
        m_DiscoveryLevels.fill(DiscoveryLevel::Unknown);
        m_DiscoveryLevels[static_cast<size_t>(PetKind::Hero)] = DiscoveryLevel::Studied;
    }

    DiscoveryLevel Bestiary::GetLevel(PetKind kind) const {
        return m_DiscoveryLevels.at(GetIndex(kind));
    }

    void Bestiary::Identify(PetKind kind) {
        auto& current = m_DiscoveryLevels.at(GetIndex(kind));
        if(current == DiscoveryLevel::Unknown) {
            current = DiscoveryLevel::Identified;
        }
    }

    void Bestiary::Study(PetKind kind) {
        m_DiscoveryLevels.at(GetIndex(kind)) = DiscoveryLevel::Studied;
    }

    bool Bestiary::Hint(PetKind kind) {
        auto& current = m_DiscoveryLevels.at(GetIndex(kind));
        switch(current) {
            using enum DiscoveryLevel;
            case Unknown: current = Identified; return true;
            case Identified: current = Studied; return true;
            case Studied: return false;
        }

        DR_ASSERT_MSG(false, "Unhandled DiscoveryLevel");
        return false;
    }
}