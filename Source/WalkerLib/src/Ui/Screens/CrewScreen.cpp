#include "Walker/Ui/Screens/CrewScreen.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Ui/EtaProgress.h"
#include "Walker/Ui/Selectors.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>
#include <Utilities/EnumUtils.h>

#include <imgui.h>
#include <optional>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};
    EndpointKind CurrentSearch{EndpointKind::Neighborhood};
    EndpointKind NextSearch{EndpointKind::Neighborhood};

	void RenderIdle() {
		ImGui::TextUnformatted("Idle");
		ImGui::SameLine();
		ImGui::Text("%llu", Home->Crew[CrewRole::Idle]);
	}

    void RenderJob(CrewRole role) {
        auto& crew = Home->Crew;
		auto roleStr = ToString(role);
		ImGui::PushID(static_cast<int>(role));
		ImGui::TextUnformatted(roleStr.c_str());
        ImGui::SameLine();
        ImGui::BeginDisabled(crew[role] < 1);
        if (ImGui::SmallButton("-"))  crew.TryUnassign(1, role);
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::Text("%llu", crew[role]);

        ImGui::SameLine();
        ImGui::BeginDisabled(crew[CrewRole::Idle] == 0);
        if (ImGui::SmallButton("+")) crew.TryAssign(1, role);
        ImGui::EndDisabled();

        if (auto progress = crew.GetProgress(role)) {
            ImGui::SameLine();
            ImGui::PushFont(GetFont(FontSizes::H2));
            WalkerUi::EtaProgressBar(*progress, crew.GetEta(role));
            ImGui::PopFont();
        }
        ImGui::PopID();
    }

    void RenderJobs() {
        RenderJob(CrewRole::Scout);
        static auto kinds = Enum::GetAllValues<EndpointKind>();
        auto count = static_cast<size_t>(Home->GetMaxScoutKind()) - static_cast<size_t>(EndpointKind::Neighborhood) + 1;
		auto choices = std::span<const EndpointKind>(kinds).first(count);
        if(WalkerUi::ScoutSelector("ScoutSelector", choices, NextSearch)) {
            Home->Crew.SetNextSearch(NextSearch);
        }

        // TODO: Block these behind achievements or something
        RenderJob(CrewRole::Engineer);
        RenderJob(CrewRole::Scientist);
    }
}

namespace Walker::WalkerUi::Screens::Crew {
    bool Initialize() { 
		Home = &ServiceLocator::Get().GetRequired<HomeBase>();
        return true; 
    }

    void ShutDown() {
        Home = nullptr;
    }

    void Render() {
        RenderIdle();
        RenderJobs();
    }
} // namespace Walker::WalkerUi::Screens::Crew
