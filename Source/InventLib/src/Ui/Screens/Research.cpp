#include "Invent/Ui/Screens/Research.h"
#include "Invent/Character/Society.h"
#include "Invent/Projects/Project.h"
#include "Invent/GameState/GameSettings.h"
#include "Invent/Ui/Ui.h"
#include "Invent/Ui/Components/ProjectComponent.h"

#include <DesignPatterns/ServiceLocator.h>

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