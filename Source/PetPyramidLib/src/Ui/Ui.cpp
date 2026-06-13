#include "Pets/Ui/Ui.h"

namespace Pets::Ui {
	bool Initialize() {
		return true;
	}

	void Render() {
		ImGui::Begin("PetPyramid");
		ImGui::TextUnformatted("Idle Pet Pyramid Scheme");
		ImGui::End();
	}

	void ShutDown() {
	}
} // namespace Pets::Ui
