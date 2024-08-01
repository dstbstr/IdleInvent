#include "Ui/Screens/Research.h"

#include "Ui/Ui.h"
#include "Ui/Components/ProjectComponent.h"

#include "InventLib/Character/Society.h"
#include "InventLib/Projects/Project.h"
#include "InventLib/Settings/GameSettings.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace {
    Invent::Society* Society{nullptr};
    Invent::GameSettings* gameSettings{nullptr};
}

namespace Ui::Screens::Research {
    bool Initialize() {
        Society = &ServiceLocator::Get().GetRequired<Invent::Society>();
        gameSettings = &ServiceLocator::Get().GetRequired<Invent::GameSettings>();
        return true;
    }

    void ShutDown() {
    }

    void Render() {
        ImGui::Begin("ResearchScreen", nullptr, BaseFlags);

        for(auto& project : Society->CurrentLife.Projects.at(Invent::ProjectType::Research)) {
            Ui::Components::Project::Render(Society->CurrentLife, project, gameSettings->PurchaseChoice);
        }

        ImGui::End();
    }
}