#include "Ui/Ui.h"

#include "Ui/Layout/Layout.h"
#include "Platform/Graphics.h"

#include "Core/Instrumentation/Logging.h"
#include "imgui.h"

namespace {
    bool hasInitialized {false};
}

namespace Ui {
    bool Initialize() {
        if(hasInitialized) return true;
        
        auto success = Layout::Initialize();
        DR_ASSERT_MSG(success, "Failed to initialize UI");
        hasInitialized = true;
        return success;
    }

    void ShutDown() {
        Layout::ShutDown();
        hasInitialized = false;
    }

    void Render() {
        Layout::Render();

        // static bool WantTextInputLast = false;
        // if(io.WantTextInput && !WantTextInputLast) { ShowSoftKeyboardInput(); }
        // WantTextInputLast = io.WantTextInput;
    }

} // namespace Ui