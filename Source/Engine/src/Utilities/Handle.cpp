#include "Utilities/Handle.h"

#include <atomic>

namespace Handles {
    Handle Next() {
        static std::atomic<u64> counter = 0;
        return Handle(counter.fetch_add(1, std::memory_order_relaxed));
    }
}