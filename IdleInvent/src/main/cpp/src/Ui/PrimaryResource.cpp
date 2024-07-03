#include "Ui/PrimaryResource.h"

#include "Ui/Ui.h"

#include "InventLib/Character/Life.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace {
    Invent::Life* life {nullptr};
}

namespace Ui::PrimaryResource {
    bool Initialize() {
        life = &ServiceLocator::Get().GetRequired<Invent::Life>();
        return true;
    }

    void ShutDown() {
        life = nullptr;
    }

    void Render() {
        auto primary = Invent::ResourceName::Primary;
        const auto& resource = life->Resources.at(primary);
        auto progress = static_cast<f32>(resource.Current) / static_cast<f32>(resource.Capacity);
        ImGui::Begin("Primary Resource", nullptr, BaseFlags);
        ImGui::ProgressBar(progress, ImVec2(-1, -1), std::format("{}: {}/{}", ToString(primary), resource.Current, resource.Capacity).c_str());
        ImGui::End();
    }
}