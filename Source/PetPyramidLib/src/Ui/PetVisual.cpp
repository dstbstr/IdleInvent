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
}