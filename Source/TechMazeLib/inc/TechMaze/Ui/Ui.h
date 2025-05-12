#pragma once

#include <imgui.h>
#include <string>

namespace TechMaze::Ui {
    enum struct FontSizes{H1, H2, H3, H4};
    bool Initialize();
    void Render();

    ImFont* GetFont(FontSizes font);
    static constexpr auto BaseFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoBackground;
    static constexpr auto ImageFlags = BaseFlags | ImGuiWindowFlags_NoInputs;
} // namespace TechMaze::Ui