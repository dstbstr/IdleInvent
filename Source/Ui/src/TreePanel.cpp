#include "Ui/TreePanel.h"

#include <imgui.h>

namespace Ui::TreePanel::Details {
	void DrawLine(const Connection& connection) {
		auto* drawList = ImGui::GetWindowDrawList();
        auto from = ToScreenSpace(connection.From);
        auto to = ToScreenSpace(connection.To);

		drawList->AddLine(from, to, connection.Color, connection.Thickness);
	}

	void DrawCorner(const Connection& connection) {
		auto* drawList = ImGui::GetWindowDrawList();
        auto from = ToScreenSpace(connection.From);
        auto to = ToScreenSpace(connection.To);
        auto dx = std::abs(to.x - from.x);
        auto dy = std::abs(to.y - from.y);

		ImVec2 p1, p2;
		if(dy >= dx) {
            auto mid = (from.y + to.y) / 2.f;
            p1 = ImVec2(from.x, mid);
            p2 = ImVec2(to.x, mid);
		} else {
            auto mid = (from.x + to.x) / 2.f;
            p1 = ImVec2(mid, from.y);
            p2 = ImVec2(mid, to.y);
		}

        drawList->AddLine(from, p1, connection.Color, connection.Thickness);
        drawList->AddLine(p1, p2, connection.Color, connection.Thickness);
        drawList->AddLine(p2, to, connection.Color, connection.Thickness);
	}
}
