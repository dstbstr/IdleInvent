#pragma once

#include <Platform/NumTypes.h>
#include <functional>

struct Handle {
private:
    static constexpr u64 Invalid = static_cast<u64>(-1);

public:
    u64 Value;
    constexpr void Reset() { Value = Invalid; }
    constexpr Handle() : Value(Invalid) {}
    constexpr explicit Handle(u64 value) : Value(value) {}

    constexpr bool operator==(const Handle& other) const { return Value == other.Value; }
    constexpr bool operator!=(const Handle& other) const { return Value != other.Value; }

    constexpr operator bool() const { return Value != Invalid; }
    constexpr bool IsValid() const { return Value != Invalid; }
};

namespace Handles {
    Handle Next();
}

namespace std {
    template<>
    struct hash<Handle> {
        size_t operator()(const Handle& handle) const noexcept {
            return std::hash<u64>{}(handle.Value);
        }
    };
}

struct ScopedHandle : public Handle {
    using DtorFn = std::function<void()>;
    DtorFn Destructor;

    // Move Only Type
    ScopedHandle() = delete;
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    explicit ScopedHandle(DtorFn dtor);
    ScopedHandle(u64 value, DtorFn dtor);

    ScopedHandle(ScopedHandle&& other) noexcept;
    ScopedHandle& operator=(ScopedHandle&& other) noexcept;

    ~ScopedHandle();

    Handle Get() const;
    Handle Release();
    void Destroy();
};

extern std::vector<ScopedHandle> GHandles;