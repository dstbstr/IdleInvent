#include "Walker/Ui/Screens/CrewScreen.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Ui/EtaProgress.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>

#include <imgui.h>
#include <optional>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};

	void RenderStats() {
		if (Home) {
			ImGui::TextUnformatted("Idle");
			ImGui::SameLine();
			ImGui::Text("%llu", Home->Crew[CrewRole::Idle]);
		}
	}

	void RenderEta(std::optional<Time> eta) {
		ImGui::Text("[%s]", eta ? Time::ToTimeString(eta.value()).c_str() : "Paused");
	}

	void RenderScouts() {
        auto& crew = Home->Crew;
        ImGui::TextUnformatted("Scouts");
        ImGui::SameLine();
        ImGui::BeginDisabled(crew[CrewRole::Scout] < 1);
        if (ImGui::SmallButton("-"))  crew.TryUnassign(1, CrewRole::Scout);
        ImGui::EndDisabled();

        ImGui::SameLine();
		ImGui::Text("%llu", crew[CrewRole::Scout]);

        ImGui::SameLine();
        ImGui::BeginDisabled(crew[CrewRole::Idle] == 0);
        if (ImGui::SmallButton("+")) crew.TryAssign(1, CrewRole::Scout);
        ImGui::EndDisabled();

        if(auto progress = crew.GetProgress(CrewRole::Scout)) {
            ImGui::SameLine();
            ImGui::PushFont(GetFont(FontSizes::H2));
            WalkerUi::EtaProgressBar(*progress, crew.GetEta(CrewRole::Scout));
            ImGui::PopFont();
        }
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
        RenderStats();
        RenderScouts();
    }
} // namespace Walker::WalkerUi::Screens::Crew
