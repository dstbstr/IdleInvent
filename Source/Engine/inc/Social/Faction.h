#pragma once

#include <Platform/NumTypes.h>

#include <limits>
#include <map>
#include <type_traits>
#include <utility>

namespace Social {
    struct FactionId {
        static constexpr u32 Invalid = std::numeric_limits<u32>::max();

        u32 Value{Invalid};
        [[nodiscard]] constexpr bool IsValid() const { return Value != Invalid; }

        constexpr explicit operator bool() const { return IsValid(); }
        constexpr auto operator<=>(const FactionId&) const = default;
    };

    template<typename TFaction>
        requires std::is_enum_v<TFaction>
    [[nodiscard]] constexpr FactionId ToFactionId(TFaction faction) {
        return {static_cast<u32>(std::to_underlying(faction))};
    }

    class FactionRelations {
    public:
        void Set(FactionId from, FactionId to, s8 opinion);
        void SetMutual(FactionId from, FactionId to, s8 opinion);
        [[nodiscard]] s8 Get(FactionId from, FactionId to) const;

    private:
        std::map<std::pair<FactionId, FactionId>, s8> m_Relations;
    };

}