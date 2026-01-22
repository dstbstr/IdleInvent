#include "Utilities/Handle.h"

#include <atomic>

namespace Handles {
    Handle Next() {
        static std::atomic<Handle> handle = {0};
        return handle.fetch_add(1, std::memory_order_relaxed);
    }
}