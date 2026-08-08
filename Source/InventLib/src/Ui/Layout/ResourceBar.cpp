#include "Invent/Ui/Layout/ResourceBar.h"
#include "Invent/Character/Life.h"
#include "Invent/Character/Society.h"
#include "Invent/Resources/InventResources.h"
#include "Invent/Ui/Ui.h"

#include <Resources/Resource.h>
#include <Constexpr/ConstexprStrUtils.h>
#include <DesignPatterns/ServiceLocator.h>

#include <format>

namespace {
    Invent::Life* life{nullptr};
}

namespace Ui::ResourceBar {
    bool Initialize() {
        life = &ServiceLocator::Get().GetRequired<Invent::Society>().CurrentLife;

        return true;
    }

    void ShutDown() {
        life = nullptr;
    }

    void Render() {
        ImGui::BeginTable("ResourceTable", 3);
        for(const auto& name : Invent::SecondaryResources()) {
            const auto& resource = life->Resources.at(name);
            ImGui::TableNextColumn();
            ImGui::Text("%s", std::format("{}: {}/{}", ToString(name), Constexpr::HumanReadable(resource.Current), Constexpr::HumanReadable(resource.Capacity)).c_str());
        }
        ImGui::EndTable();
    }
}