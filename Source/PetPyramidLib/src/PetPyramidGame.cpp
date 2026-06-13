#include "Pets/PetPyramidGame.h"
#include "Pets/Ui/Ui.h"

#include "Platform/Graphics.h"

namespace Pets {
	bool PetPyramidGame::Initialize() {
		return Ui::Initialize();
	}

	void PetPyramidGame::ShutDown() {
		Ui::ShutDown();
	}

	void PetPyramidGame::LoadGame() {
	}

	void PetPyramidGame::SaveGame() {
	}

	void PetPyramidGame::DeleteGame() {
	}

	void PetPyramidGame::Tick([[maybe_unused]] BaseTime elapsed) {
		Graphics::Render(Ui::Render);
	}
} // namespace Pets
