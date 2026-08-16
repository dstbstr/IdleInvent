#include "World/Movement.h"

namespace World {
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {0.0f, 0.0f}) == WorldLocation{{0, 0}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {1.0f, 1.0f}) == WorldLocation{{0, 0}, {1.0f, 1.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {16.0f, 0.0f}) == WorldLocation{{1, 0}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {0.0f, 16.0f}) == WorldLocation{{0, 1}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {-16.0f, 0.0f}) == WorldLocation{{-1, 0}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {0.0f, -16.0f}) == WorldLocation{{0, -1}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {32.0f, 32.0f}) == WorldLocation{{2, 2}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {-32.0f, -32.0f}) == WorldLocation{{-2, -2}, {0.0f, 0.0f}});

    constexpr bool TryMoveTests() {
        auto AlwaysTrue = [](WorldLocation) { return true; };
        auto AlwaysFalse = [](WorldLocation) { return false; };
    
        auto start = WorldLocation{{0, 0}, {0.0f, 0.0f}};
        auto offset = Displacement{1.0f, 1.0f};

        if(TryMove<16, 16>(start, offset, AlwaysFalse)) return false;
        if(start != WorldLocation{{0, 0}, {0.0f, 0.0f}}) return false;

        if(TryMove<16, 16>(start, offset, AlwaysTrue) == false) return false;
        if(start != WorldLocation{{0, 0}, {1.0f, 1.0f}}) return false;

        if(TryMove<16, 16>(start, Displacement{15.0f, 15.0f}, AlwaysTrue) == false) return false;
        if(start != WorldLocation{{1, 1}, {0.0f, 0.0f}}) return false;

        return true;
    }

    static_assert(TryMoveTests());
}