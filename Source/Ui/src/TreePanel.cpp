#include "Ui/TreePanel.h"

#include <cmath>
#include <imgui.h>

namespace Ui::Details {
    ImVec2 ToScreenSpace(const ImVec2& localPos) {
        const auto screenCursor = ImGui::GetCursorScreenPos();
        const auto localCursor = ImGui::GetCursorPos();
        const ImVec2 origin{screenCursor.x - localCursor.x, screenCursor.y - localCursor.y};
        return {origin.x + localPos.x, origin.y + localPos.y};
    }

	void DrawLine(const Connection& connection) {
		auto* drawList = ImGui::GetWindowDrawList();
		const auto from = ToScreenSpace(connection.From);
		const auto to = ToScreenSpace(connection.To);

		drawList->AddLine(from, to, connection.Color, connection.Thickness);
	}

	void DrawCorner(const Connection& connection) {
		auto* drawList = ImGui::GetWindowDrawList();
		const auto from = ToScreenSpace(connection.From);
		const auto to = ToScreenSpace(connection.To);
		const auto dx = std::abs(to.x - from.x);
		const auto dy = std::abs(to.y - from.y);

		ImVec2 p1, p2;
		if(dy >= dx) {
			const auto mid = (from.y + to.y) / 2.f;
			p1 = ImVec2(from.x, mid);
			p2 = ImVec2(to.x, mid);
		} else {
			const auto mid = (from.x + to.x) / 2.f;
			p1 = ImVec2(mid, from.y);
			p2 = ImVec2(mid, to.y);
		}

		drawList->AddLine(from, p1, connection.Color, connection.Thickness);
		drawList->AddLine(p1, p2, connection.Color, connection.Thickness);
		drawList->AddLine(p2, to, connection.Color, connection.Thickness);
	}
}
