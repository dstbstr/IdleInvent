#include "Walker/Ui/Screens/MilestonesScreen.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Milestones/Milestones.h"

#include <Ui/UiUtil.h>
#include <imgui.h>

#include <format>
#include <optional>
#include <string>

namespace {
    using namespace Walker;
	MilestoneKind SelectedMilestone{ MilestoneKind::Unset };
    HomeBase* Home{nullptr};
    MilestoneManager* Milestones{nullptr};

    void RenderCard(MilestoneKind kind, const char* subtitle) {
        const auto& presentation = GetMilestonePresentation(kind);
        auto unlocked = Milestones->IsUnlocked(kind);
        auto hidden = presentation.Secret && !unlocked;
        auto selected = SelectedMilestone == kind;

        auto label = hidden ? "?" : presentation.Name;
		if (!hidden && subtitle && subtitle[0] != '\0') {
            label += "\n";
            label += subtitle;
		}
        label += "###Card";

        ImGui::PushID(static_cast<int>(kind));
		auto size = ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 3.f };
        PushFittedFont(FontSizes::H3, label.c_str(), size, std::nullopt, 2.f);

        ImGui::PushStyleColor(ImGuiCol_Button, unlocked 
            ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) 
            : ImGui::GetStyleColorVec4(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_Border, selected
			? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)
			: ImGui::GetStyleColorVec4(ImGuiCol_Border));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, selected ? 2.f : 1.f);

        if(ImGui::Button(label.c_str(), size)) {
			SelectedMilestone = selected ? MilestoneKind::Unset : kind;
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
        ImGui::PopFont();
        ImGui::PopID();
    }

    void RenderTieredMilestones() {
		if (ImGui::BeginTable("TieredCards", 4, ImGuiTableFlags_SizingStretchSame)) {
			for (const auto& details : GetTieredMilestones()) {
				ImGui::TableNextColumn();
				auto text = std::format("{}/{}", Milestones->GetUnlockedTier(details.Kind), details.Thresholds.size());
				RenderCard(details.Kind, text.c_str());
			}
			ImGui::EndTable();
		}
    }

    void RenderOneTimeMilestones() {
		if (ImGui::BeginTable("OneTimeCards", 4, ImGuiTableFlags_SizingStretchSame)) {
			for (const auto& details : GetOneTimeMilestones()) {
				ImGui::TableNextColumn();
				RenderCard(details.Kind, Milestones->IsUnlocked(details.Kind) ? "Unlocked" : "Locked");
			}
		    ImGui::EndTable();
	    }
    }

    void RenderSelectedDetails() {
		if (SelectedMilestone == MilestoneKind::Unset) {
            ImGui::TextUnformatted("Select a milestone");
            return;
        }
    }
}

namespace Walker::WalkerUi::Screens::Milestones {
    bool Initialize() {
		Home = &ServiceLocator::Get().GetRequired<HomeBase>();
		::Milestones = &Home->Milestones;
        return true;
    }

    void ShutDown() {
        ::Milestones = nullptr;
    }

    void Render() {
		ImGui::SeparatorText("Tiered Milestones");
		RenderTieredMilestones();
        ImGui::SeparatorText("One-Time Milestones");
		RenderOneTimeMilestones();

        
		RenderSelectedDetails();
    }
}
