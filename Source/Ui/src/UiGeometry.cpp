#include "Ui/UiGeometry.h"

#include <array>

namespace Ui {
    bool SliderScale2(const char* label, Geometry::Scale2& value, f32 min, f32 max) {
        std::array<f32, 2> arr{value.X, value.Y};
        if(ImGui::SliderFloat2(label, arr.data(), min, max)) {
            value.X = arr.at(0);
            value.Y = arr.at(1);
            return true;
        }

        return false;
    }
}