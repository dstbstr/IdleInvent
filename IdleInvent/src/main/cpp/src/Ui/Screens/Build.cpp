#include "Ui/Screens/Build.h"

#include "Ui/Ui.h"
#include "Ui/Components/ProjectComponent.h"

#include "InventLib/Character/Society.h"
#include "InventLib/Projects/Project.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace {
    Invent::Society* Society{nullptr};
}

namespace Ui::Screens::Build {
    bool Initialize() {
        Society = &ServiceLocator::Get().GetRequired<Invent::Society>();
        return true;
    }

    void ShutDown() {
    }

    void Render() {
        ImGui::Begin("BuildScreen", nullptr, BaseFlags);

        for(auto& project : Society->CurrentLife.Projects.at(Invent::ProjectType::Build)) {
            if(ImGui::CollapsingHeader(std::format("{} ({})", project.Name, Society->CurrentLife.Buildings.at(project.Name)).c_str())) {
                Ui::Components::Project::Render(Society->CurrentLife, project);
                ImGui::Separator();
            }
        }

        ImGui::End();
    }
}