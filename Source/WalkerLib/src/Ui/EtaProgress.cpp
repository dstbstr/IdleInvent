#include "Walker/Ui/EtaProgress.h"

namespace Walker::WalkerUi {
	void EtaProgressBar(f32 progress, std::optional<Time> eta, ImVec2 size) {
		auto text = eta ? Time::ToTimeString(eta.value()) : "Paused";
		ImGui::ProgressBar(progress, size, "");

		auto min = ImGui::GetItemRectMin();
		auto max = ImGui::GetItemRectMax();
		auto textSize = ImGui::CalcTextSize(text.c_str());
		auto pos = ImVec2{
			min.x + (max.x - min.x - textSize.x) * 0.5f,
			min.y + (max.y - min.y - textSize.y) * 0.5f
		};

		auto* drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(min, max, true);
		drawList->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), text.c_str());
		drawList->PopClipRect();
	}
}