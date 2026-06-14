#include "Pets/PetPyramidGame.h"
#include "Pets/Ui/Ui.h"

#include "DesignPatterns/ServiceLocator.h"
#include "Manage/TickManager.h"
#include "Platform/Graphics.h"
#include "Animation/Animation.h"

namespace Pets {
	bool PetPyramidGame::Initialize() {
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<TickManager>();
        services.CreateIfMissing<std::unordered_map<std::string, Animation>>();

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

	void PetPyramidGame::Tick(BaseTime elapsed) {
        TickManager::Get().Tick(elapsed);
		Graphics::Render(Ui::Render);
	}
} // namespace Pets
