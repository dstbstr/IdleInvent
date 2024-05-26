#pragma once

#include <imgui.h>

namespace Ui {
    void Render();

    static constexpr auto BaseFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
    static constexpr auto ImageFlags = BaseFlags | ImGuiWindowFlags_NoInputs;
}