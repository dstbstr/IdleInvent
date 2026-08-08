#include "Invent/Ui/Ui.h"
#include "Invent/Ui/Layout/Layout.h"

#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <imgui.h>

namespace {
    bool hasInitialized {false};
    constexpr auto IconSprites = "Icons/Icons.png";
}

namespace Ui {
    bool Initialize() {
        if(hasInitialized) return true;
        DR_ASSERT(Graphics::TryLoadSpriteSheet(IconSprites));

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