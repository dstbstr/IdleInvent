#include "Pets/PetPyramidGame.h"
#include "Pets/Combat/HuntManager.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Inventory/Inventory.h"

#include <DesignPatterns/PubSub.h>
#include "DesignPatterns/ServiceLocator.h"
#include "Manage/TickManager.h"
#include "Platform/Graphics.h"
#include "Animation/Animation.h"

namespace {
	std::vector<ScopedHandle> GlobalSubs{};
}

namespace Pets {
	bool PetPyramidGame::Initialize() {
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<TickManager>();
        services.CreateIfMissing<std::unordered_map<std::string, Animation>>();
        auto& inv = services.GetOrCreate<Inventory>();
		auto searchTime = OneSecond;
		auto& hunt = services.GetOrCreate<HuntManager>(inv, searchTime);
		TickManager::Get().Register(GlobalSubs, hunt);

		return Ui::Initialize();
	}

	void PetPyramidGame::ShutDown() {
		Ui::ShutDown();
		GlobalSubs.clear();
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
