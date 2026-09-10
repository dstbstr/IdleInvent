#include "Platform/Graphics.h"
#include "GhostHunter/UI/MainContent.h"
#include "GhostHunter/UI/Ui.h"
#include "GhostHunter/UI/Screens/Screens.h"

#include <Ui/ChildBuilder.h>
#include <Ui/UiUtil.h>

#include <array>

namespace GhostHunter::Ui::Screens::MainContent {
    bool Initialize() {
        return Screens::Initialize();
    }
    void Render() {
        Screens::Render();
    }
    void ShutDown() {
        Screens::ShutDown(); 
    }
} // namespace GhostHunter::Ui::Screens::MainContent