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
        auto& party = services.GetOrCreate<Party>();
		auto& petRoster = services.GetOrCreate<PetRoster>();
		auto searchTime = OneSecond;
		auto& hunt = services.GetOrCreate<HuntManager>(inv, party, petRoster, searchTime);
		TickManager::Get().Register(GlobalSubs, hunt);

		petRoster[PetKind::Hero] = OwnedPet {
			.Kind = PetKind::Hero,
			.Level = 1,
			.Experience = 10
		};

		petRoster[PetKind::Poodle] = OwnedPet {
			.Kind = PetKind::Poodle,
			.Level = 1,
			.Experience = 100
		};
		petRoster[PetKind::Bloodhound] = OwnedPet {
			.Kind = PetKind::Bloodhound,
			.Level = 10,
			.Experience = 1'000'000
		};

		party.Hero.Pets.push_back({.Kind = PetKind::Poodle});
		party.Hero.Pets[0].Pets.push_back({.Kind = PetKind::Bloodhound});
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
