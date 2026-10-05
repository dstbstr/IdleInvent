#include "Walker/Ui/Screens/CrewScreen.h"
#include "Walker/WalkerSettings.h"
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
    WalkerSettings* Settings{nullptr};
    EndpointKind CurrentSearch{EndpointKind::Neighborhood};
    EndpointKind NextSearch{EndpointKind::Neighborhood};

	void RenderIdle() {
		ImGui::TextUnformatted("Idle");
		ImGui::SameLine();
		ImGui::Text("%llu", Home->Crew[CrewRole::Idle]);

        auto affordable = Home->Rates.GetMaxHireCount(Home->Funds.GetBalance(), Home->Crew.GetHiredCount());
        auto purchaseCount = GetPurchaseCount(affordable, Settings->PurchaseSetting);
        ImGui::BeginDisabled(purchaseCount == 0);
        ImGui::SameLine();
		auto label = "+ " + std::to_string(purchaseCount);
        if(ImGui::SmallButton(label.c_str())) {
            Home->TryHireCrew(purchaseCount);
        }
        ImGui::EndDisabled();
	}

    void RenderJob(CrewRole role) {
        auto& crew = Home->Crew;
		auto roleStr = ToString(role);
		ImGui::PushID(static_cast<int>(role));
		ImGui::TextUnformatted(roleStr.c_str());
        ImGui::SameLine();
        auto removable = GetPurchaseCount(crew[role], Settings->PurchaseSetting);
        ImGui::BeginDisabled(removable == 0);
        if (ImGui::SmallButton("-"))  crew.TryUnassign(removable, role);
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::Text("%llu", crew[role]);

        ImGui::SameLine();
		auto addable = GetPurchaseCount(crew[CrewRole::Idle], Settings->PurchaseSetting);
        ImGui::BeginDisabled(addable == 0 || !crew.GetProgress(role).has_value());
        if (ImGui::SmallButton("+")) crew.TryAssign(addable, role);
        ImGui::EndDisabled();

        if (auto progress = crew.GetProgress(role)) {
            ImGui::SameLine();
            ImGui::PushFont(GetFont(FontSizes::H2));
            WalkerUi::EtaProgressBar(*progress, crew.GetEta(role));
            ImGui::PopFont();
        }
        ImGui::PopID();
    }

    void RenderTechnologies() {
        auto& crew = Home->Crew;
        if(ImGui::BeginTable("Technologies", 2, ImGuiTableFlags_SizingStretchSame)) {
            auto Technology = [&](TechKind kind) {
                const auto& state = Home->Tech[kind];
				auto role = state.Researched ? CrewRole::Engineer : CrewRole::Scientist;
                auto name = ToString(kind);

                ImGui::TableNextColumn();
                ImGui::PushID(static_cast<int>(kind));
                ImGui::TextUnformatted(name.c_str());

                auto busy = crew.GetProgress(role).has_value();
                auto upgradeBlocked = state.Researched && !Home->Tech.CanUpgrade(kind);
                ImGui::BeginDisabled(busy || upgradeBlocked);
				if (ImGui::Button(state.Researched ? "Improve" : "Research", ImVec2{ -1.f, 0.f })) {
					if (state.Researched) {
						crew.TryStartEngineering(kind);
					}
					else {
						crew.TryStartScience(kind);
					}
				}
                ImGui::EndDisabled();
                if(state.Researched) {
                    ImGui::Text("Level %llu", state.CurrentLevel);
                } else {
                    ImGui::TextUnformatted("Unresearched");
                }
                ImGui::PopID();
            };

            for(auto kind : Enum::GetAllValues<TechKind>()) {
                Technology(kind);
            }

            ImGui::EndTable();
        }
    }

    void RenderJobs() {
        RenderJob(CrewRole::Scout);
        static auto kinds = Enum::GetAllValues<EndpointKind>();
        auto count = static_cast<size_t>(Home->GetMaxScoutKind()) - static_cast<size_t>(EndpointKind::Neighborhood) + 1;
		auto choices = std::span<const EndpointKind>(kinds).first(count);

        auto& crew = Home->Crew;
        if(WalkerUi::ScoutSelector("ScoutSelector", choices, NextSearch)) {
            crew.SetNextSearch(NextSearch);
        }

        // TODO: Block these behind achievements or something
        RenderTechnologies();
        RenderJob(CrewRole::Scientist);
        RenderJob(CrewRole::Engineer);
    }
}

namespace Walker::WalkerUi::Screens::Crew {
    bool Initialize() { 
		auto& services = ServiceLocator::Get();
		Home = &services.GetRequired<HomeBase>();
		Settings = &services.GetRequired<WalkerSettings>();
        return true; 
    }

    void ShutDown() {
        Home = nullptr;
        Settings = nullptr;
    }

    void Render() {
        RenderIdle();
        RenderJobs();
    }
} // namespace Walker::WalkerUi::Screens::Crew
