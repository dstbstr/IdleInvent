#include "Invent/Ui/Layout/PrimaryResource.h"
#include "Invent/Ui/Ui.h"
#include "Invent/Character/Life.h"
#include "Invent/Character/Society.h"
#include "Invent/Resources/InventResources.h"

#include <DesignPatterns/ServiceLocator.h>

namespace {
    Invent::Life* life {nullptr};
}

namespace Ui::PrimaryResource {
    bool Initialize() {
        life = &ServiceLocator::Get().GetRequired<Invent::Society>().CurrentLife;
        return true;
    }

    void ShutDown() {
        life = nullptr;
    }

    void Render() {
        auto primary = Invent::ResourceName::Primary;
        const auto& resource = life->Resources.at(primary);
        auto progress = static_cast<f32>(resource.Current) / static_cast<f32>(resource.Capacity);
        ImGui::ProgressBar(progress, ImVec2(-1, -1), std::format("Work: {}/{}", resource.Current, resource.Capacity).c_str());
    }
}