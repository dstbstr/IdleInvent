#include "Pets/Inventory/Inventory.h"

#include <Instrumentation/Logging.h>
#include <algorithm>

namespace Pets {
    bool Inventory::Contains(CombatItemKind item, size_t qty) const { 
        return m_CombatItems.contains(item) && m_CombatItems.at(item) >= qty;
    }
    
    void Inventory::Consume(CombatItemKind item, size_t qty) { 
        DR_ASSERT_MSG(Contains(item, qty), "Insufficient items");
        m_CombatItems[item] -= std::min(m_CombatItems[item], qty);
    }
    
    void Inventory::Add(CombatItemKind item, size_t qty) { 
        m_CombatItems[item] += qty;
    }
    
    size_t Inventory::Count(CombatItemKind item) const { 
        if (m_CombatItems.contains(item)) {
            return m_CombatItems.at(item);
        }
        return 0z;
    }
    
    bool Inventory::Contains(FieldItemKind item, size_t qty) const {
        return m_FieldItems.contains(item) && m_FieldItems.at(item) >= qty;
    }
    
    void Inventory::Consume(FieldItemKind item, size_t qty) {
        DR_ASSERT_MSG(Contains(item, qty), "Insufficient items");
        m_FieldItems[item] -= std::min(m_FieldItems[item], qty);
    }
    
    void Inventory::Add(FieldItemKind item, size_t qty) {
        m_FieldItems[item] += qty;
    }
    
    size_t Inventory::Count(FieldItemKind item) const {
        if (m_FieldItems.contains(item)) {
            return m_FieldItems.at(item);
        }
        return 0z;
    }
}