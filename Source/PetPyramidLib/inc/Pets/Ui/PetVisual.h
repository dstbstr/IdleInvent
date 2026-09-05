#pragma once

#include "Pets/Pets/Pets.h"
#include <Ui/UiGeometry.h>

#include <imgui.h>

namespace Pets {
	enum struct Shape {
		Circle, Square, Triangle
	};
	enum struct ShapeFill {
		Solid, Outline, Hollow
	};
	struct PetVisual {
		// temporary until textures or animations are created
		Shape PetShape{Shape::Circle};
		ShapeFill Fill{ShapeFill::Solid};
		ImVec4 BaseColor{};
	};

	PetVisual GetVisual(PetKind pet);
	void RenderVisualStill(const PetVisual& visual, const ::Ui::UiRect& bounds, ImDrawList* drawList = nullptr);
}