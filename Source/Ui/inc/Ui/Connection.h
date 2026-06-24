#pragma once
#include "Platform/NumTypes.h"
#include <imgui.h>

namespace Ui {
    enum struct ConnectPoint : u8 { North, East, South, West, NorthEast, NorthWest, SouthEast, SouthWest };
    enum struct ConnectStyle : u8 { None, Line, Corner };

    struct Connection {
        ImVec2 From;
        ImVec2 To;
        f32 Thickness{1.f};
        ImU32 Color{0xFFFFFFFF};
    };
}