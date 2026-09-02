#pragma once

#include "Pets/Inventory/Items.h"

#include <Combat/CombatTypes.h>
#include <Platform/NumTypes.h>

#include <variant>

namespace Pets {
    enum struct ActionRequestKind : u8 {
        // Party actions
        Attack,
        Capture,
        Item,

        // Prey actions
        Defend,
        Flee,
        Hide,
        LookPretty
    };

    struct ItemContext {
        CombatItemKind ItemId{};
    };

    using ActionRequestContext = std::variant<std::monostate, ItemContext>;
    struct ActionRequest {
        ActionRequestKind Kind{};
        Combat::CombatantId Target{};
        ActionRequestContext Context{};
    };

    enum struct ActionResultKind : u8 {
        Damaged,
        Captured,
        CaptureFailed,
        Defended,
        Hidden,
        ItemUsed,
        PreyFled,
        PreyKilled,
        TurnEnded,
        TurnSkipped
    };

    using ActionResultContext = std::variant<std::monostate, s32, CombatItemKind>;
    struct ActionResult {
        ActionResultKind Kind{};
        Combat::CombatantId Actor{};
        Combat::CombatantId Target{};
        ActionResultContext Context{};
    };

}