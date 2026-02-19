#include "Utilities/Handle.h"

#include <atomic>

namespace Handles {
    Handle Next() {
        static std::atomic<u64> counter = 0;
        return Handle(counter.fetch_add(1, std::memory_order_relaxed));
    }
}

ScopedHandle::ScopedHandle(DtorFn dtor) : Handle(), Destructor(std::move(dtor)) {}
ScopedHandle::ScopedHandle(u64 value, DtorFn dtor) : Handle(value), Destructor(std::move(dtor)) {}

ScopedHandle::ScopedHandle(ScopedHandle&& other) noexcept : Handle(other.Value), Destructor(std::move(other.Destructor)) {
    other.Reset();
}
ScopedHandle& ScopedHandle::operator=(ScopedHandle&& other) noexcept {
    if(this != &other) {
        Value = other.Value;
        Destructor = std::move(other.Destructor);
        other.Reset();
    }
    return *this;
}

ScopedHandle::~ScopedHandle() { Destroy(); }

Handle ScopedHandle::Get() const { return Handle(Value); }

Handle ScopedHandle::Release() {
    Handle handle = Handle(Value);
    Reset();
    return handle;
}

void ScopedHandle::Destroy() {
    if(IsValid() && Destructor) {
        Destructor();
        Reset();
    }
}

std::vector<ScopedHandle> GHandles{};

static_assert(std::is_move_assignable_v<ScopedHandle>);
static_assert(std::is_move_constructible_v<ScopedHandle>);
static_assert(!std::is_copy_assignable_v<ScopedHandle>);
static_assert(!std::is_copy_constructible_v<ScopedHandle>);
static_assert(!std::is_default_constructible_v<ScopedHandle>);