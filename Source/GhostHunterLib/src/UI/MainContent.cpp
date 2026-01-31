#include "Platform/Graphics.h"
#include "GhostHunter/Ui/MainContent.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Ui/Screens/Screens.h"

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