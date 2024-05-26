#include "Platform/Inputs.h"

#include "backends/imgui_impl_android.h"

namespace Inputs {
    int HandleInput(void* event) {
        return ImGui_ImplAndroid_HandleInputEvent(static_cast<AInputEvent*>(event));
    }
}