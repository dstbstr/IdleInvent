#include "Ui/Screens/Home.h"

#include "Ui/Ui.h"

#include "InventLib/GameState/GameState.h"
#include "InventLib/Character/Life.h"

#include "Core/Constexpr/ConstexprStrUtils.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

#include <format>

namespace {
    Invent::Life* life{nullptr};
}

namespace Ui::Screens::Home {
    bool Initialize() {
        life = &ServiceLocator::Get().GetRequired<Invent::Life>();
        return true;
    }

    void ShutDown() {}
    
    void Render() { 
        ImGui::Begin("Home", nullptr, BaseFlags);

        // Buttons to convert resources into other resources

        ImGui::End();
    }
}