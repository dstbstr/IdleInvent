#pragma once

#include "Pets/Inventory/Items.h"

#include <Platform/NumTypes.h>

#include <span>
#include <string_view>

namespace Pets {

	template<typename TKind>
	struct ItemDetails {
        TKind Kind{};
        std::string_view Name{};
        u64 Cost{};
	};

	using CombatItemDetails = ItemDetails<CombatItemKind>;
    using FieldItemDetails = ItemDetails<FieldItemKind>;

    namespace Details {
        [[nodiscard]] const CombatItemDetails& GetItem(CombatItemKind kind);
        [[nodiscard]] const FieldItemDetails& GetItem(FieldItemKind kind);

        [[nodiscard]] std::span<const CombatItemDetails> GetCombatItems();
        [[nodiscard]] std::span<const FieldItemDetails> GetFieldItems();
    } // namespace Details
}