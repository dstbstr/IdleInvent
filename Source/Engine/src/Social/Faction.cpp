#include "Social/Faction.h"

namespace Social {
    void FactionRelations::Set(FactionId from, FactionId to, s8 opinion) {
        m_Relations[{from, to}] = opinion;
    }

    void FactionRelations::SetMutual(FactionId from, FactionId to, s8 opinion) {
        Set(from, to, opinion);
        Set(to, from, opinion);
    }

    [[nodiscard]] s8 FactionRelations::Get(FactionId from, FactionId to) const {
        return m_Relations.at({from, to});
    }
}