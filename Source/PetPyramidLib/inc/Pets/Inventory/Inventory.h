#pragma once

#include "Pets/Inventory/Items.h"

#include <map>
namespace Pets {
    class Inventory {
    public:
        bool Contains(CombatItemKind item, size_t qty = 1) const;
        void Consume(CombatItemKind item, size_t qty = 1);
        void Add(CombatItemKind item, size_t qty = 1);
        size_t Count(CombatItemKind item) const;

        bool Contains(FieldItemKind item, size_t qty = 1) const;
        void Consume(FieldItemKind item, size_t qty = 1);
        void Add(FieldItemKind item, size_t qty = 1);
        size_t Count(FieldItemKind item) const;

    private:
        std::map<CombatItemKind, size_t> m_CombatItems{};
        std::map<FieldItemKind, size_t> m_FieldItems{};
    };

}