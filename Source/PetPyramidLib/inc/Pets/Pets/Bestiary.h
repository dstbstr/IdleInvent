#pragma once

#include "Pets/Pets/Pets.h"

#include <array>

namespace Pets {
    enum struct DiscoveryLevel { Unknown, Identified, Studied };
    class Bestiary {
    public:
        Bestiary();
        ~Bestiary() = default;

        DiscoveryLevel GetLevel(PetKind kind) const;
        void Identify(PetKind kind);
        void Study(PetKind kind);
        bool Hint(PetKind kind);

    private:
        std::array<DiscoveryLevel, static_cast<size_t>(PetKind::COUNT)> m_DiscoveryLevels{};
    };
}