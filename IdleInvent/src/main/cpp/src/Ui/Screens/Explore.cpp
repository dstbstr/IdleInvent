#include "Ui/Screens/Explore.h"

#include "Ui/Ui.h"
#include "Ui/Components/ProjectComponent.h"

#include "InventLib/Character/Society.h"
#include "InventLib/Projects/Project.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace {
    Invent::Society* Society{nullptr};
}

namespace Ui::Screens::Explore {
    bool Initialize() {
        Society = &ServiceLocator::Get().GetRequired<Invent::Society>();
        return true;
    }

    void ShutDown() {
    }

    void Render() {
        ImGui::Begin("ExploreScreen", nullptr, BaseFlags);

        for(auto& project : Society->CurrentLife.Projects.at(Invent::ProjectType::Explore)) {
            if(ImGui::CollapsingHeader(project.Name.c_str())) {
                Ui::Components::Project::Render(Society->CurrentLife, project, Ui::Components::Project::StartCondition::RequireResources);
                ImGui::Separator();
            }
        }

        ImGui::End();
    }
}