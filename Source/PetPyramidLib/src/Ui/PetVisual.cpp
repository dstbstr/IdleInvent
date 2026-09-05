#include "Pets/Ui/PetVisual.h"
#include <array>

namespace {
	constexpr std::array<ImVec4, 9> PetColors {
		ImVec4{0.906f, 0.435f, 0.318f, 1.f},
		ImVec4{0.957f, 0.635f, 0.380f, 1.f},
		ImVec4{0.914f, 0.769f, 0.416f, 1.f},
		ImVec4{0.447f, 0.710f, 0.604f, 1.f},
		ImVec4{0.165f, 0.616f, 0.561f, 1.f},
		ImVec4{0.333f, 0.659f, 0.851f, 1.f},
		ImVec4{0.443f, 0.537f, 0.851f, 1.f},
		ImVec4{0.608f, 0.447f, 0.812f, 1.f},
		ImVec4{0.851f, 0.408f, 0.627f, 1.f}
	};
}
namespace Pets {
	PetVisual GetVisual(PetKind kind) {
		auto index = static_cast<size_t>(kind);

		auto colorIndex = index % PetColors.size();
		auto shapeIndex = (index / PetColors.size()) % 3;
		auto fillIndex = ((index / PetColors.size()) / 3) % 3;
		return PetVisual{
			static_cast<Shape>(shapeIndex),
			static_cast<ShapeFill>(fillIndex),
			PetColors[colorIndex]
		};
	}

	void RenderVisualStill(const PetVisual& visual, const ::Ui::UiRect& bounds, ImDrawList* drawList) {
        if(!drawList) {
            drawList = ImGui::GetWindowDrawList();
        }

        auto center = bounds.GetCenter();
        auto halfSize = bounds.GetSize() * 0.5f;
        auto radius = std::min(halfSize.x, halfSize.y);
        auto r2 = ImVec2(radius, radius);

        auto min = center - r2;
        auto max = center + r2;
        auto color = ImGui::ColorConvertFloat4ToU32(visual.BaseColor);

        switch(visual.PetShape) {
            using enum Pets::Shape;
            case Circle: {
                switch(visual.Fill) {
                    using enum Pets::ShapeFill;
                case Solid: drawList->AddCircleFilled(center, radius, color); break;
                case Outline:
                    drawList->AddCircleFilled(center, radius, color);
                    drawList->AddCircle(center, radius, IM_COL32_BLACK);
                    break;
                case Hollow: drawList->AddCircle(center, radius, color); break;
                }
                break;
            }
            case Square: {
                switch(visual.Fill) {
                    using enum Pets::ShapeFill;
                case Solid: drawList->AddRectFilled(min, max, color); break;
                case Outline:
                    drawList->AddRectFilled(min, max, color);
                    drawList->AddRect(min, max, IM_COL32_BLACK);
                    break;
                case Hollow: drawList->AddRect(min, max, color); break;
                }
                break;
            }
            case Triangle: {
                auto top = ImVec2{center.x, min.y};
                auto left = ImVec2{min.x, max.y};
                auto right = ImVec2{max.x, max.y};
                switch(visual.Fill) {
                    using enum Pets::ShapeFill;
                case Solid: drawList->AddTriangleFilled(top, left, right, color); break;
                case Outline:
                    drawList->AddTriangleFilled(top, left, right, color);
                    drawList->AddTriangle(top, left, right, IM_COL32_BLACK);
                    break;
                case Hollow: drawList->AddTriangle(top, left, right, color); break;
                }
                break;
            }
	    }
    }
}