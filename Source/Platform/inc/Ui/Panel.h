#pragma once
#include "Platform/NumTypes.h"
#include <utility>
#include <vector>
#include <memory>

// Top third, full width = Top | Left | Right | HCenter
// Right 2/3, full height = HCenter | Right | Top | Bottom | VCenter
// Top Left Corner = Top | Left
enum struct PanelPos {
    FullScreen = 0,
    Top = 1, 
    Bottom = 1 << 1, 
    VCenter = 1 << 2,
    Left = 1 << 3,
    Right = 1 << 4,
    HCenter = 1 << 5
};


struct Panel {
    Panel(PanelPos pos, const std::pair<f32, f32>& parentDimensions);
    virtual ~Panel() = default;
    virtual void Render() = 0;
    std::pair<f32, f32> GetDimensions();

private:
    std::vector<std::unique_ptr<Panel>> m_Children{};
    std::pair<f32, f32> m_Dimensions{0.0f, 0.0f};
};
