#include "Ui/ResourceBar.h"

#include "Ui/Ui.h"

#include "InventLib/Resources/Resource.h"
#include "InventLib/Character/Life.h"

#include "Core/Constexpr/ConstexprStrUtils.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include <format>

namespace {
    Invent::Life* life{nullptr};
}

namespace Ui::ResourceBar {
    bool Initialize() {
        life = &ServiceLocator::Get().GetRequired<Invent::Life>();

        return true;
    }

    void ShutDown() {
        life = nullptr;
    }

    void Render() {
        ImGui::Begin("ResourceBar", nullptr, BaseFlags);
        ImGui::BeginTable("ResourceTable", 3);
        for(const auto& name : Invent::SecondaryResources) {
            const auto& resource = life->Resources.at(name);
            ImGui::TableNextColumn();
            ImGui::Text("%s", std::format("{}: {}", ToString(name), Constexpr::HumanReadable(resource.Current)).c_str());
        }
        ImGui::EndTable();
        ImGui::End();
    }
}