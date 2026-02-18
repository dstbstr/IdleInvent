#include "Utilities/Handle.h"

#include <atomic>

namespace Handles {
    Handle Next() {
        static std::atomic<u64> counter = 0;
        return Handle(counter.fetch_add(1, std::memory_order_relaxed));
    }
}

std::vector<ScopedHandle> GHandles{};

static_assert(std::is_move_assignable_v<ScopedHandle>);
static_assert(std::is_move_constructible_v<ScopedHandle>);
static_assert(!std::is_copy_assignable_v<ScopedHandle>);
static_assert(!std::is_copy_constructible_v<ScopedHandle>);
static_assert(!std::is_default_constructible_v<ScopedHandle>);