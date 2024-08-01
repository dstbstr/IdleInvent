#include "Ui/Screens/Build.h"

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

namespace Ui::Screens::Build {
    bool Initialize() {
        Society = &ServiceLocator::Get().GetRequired<Invent::Society>();
        gameSettings = &ServiceLocator::Get().GetRequired<Invent::GameSettings>();
        return true;
    }

    void ShutDown() {
    }

    void Render() {
        ImGui::Begin("BuildScreen", nullptr, BaseFlags);

        for(auto& project : Society->CurrentLife.Projects.at(Invent::ProjectType::Build)) {
            if(ImGui::CollapsingHeader(std::format("{} ({})", project.Name, Society->CurrentLife.Buildings.at(project.Name)).c_str()), ImGuiTreeNodeFlags_DefaultOpen) {
                Ui::Components::Project::Render(Society->CurrentLife, project, gameSettings->PurchaseChoice);
                ImGui::Separator();
            }
        }

        ImGui::End();
    }
}