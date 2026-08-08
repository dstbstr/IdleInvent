#pragma once

#include <imgui.h>

namespace Ui {
    bool Initialize();
    void Render();
    void ShutDown();

    static constexpr auto BaseFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    static constexpr auto ImageFlags = BaseFlags | ImGuiWindowFlags_NoInputs;
}