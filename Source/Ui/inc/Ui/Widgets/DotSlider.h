#pragma once

#include <Platform/NumTypes.h>
#include <imgui.h>

namespace Ui {
    void DotSlider(const char* id, 
        f32& position, 
        const char* leftLabel = "", 
        const char* rightLabel = "", 
        size_t tickCount = 0);
}