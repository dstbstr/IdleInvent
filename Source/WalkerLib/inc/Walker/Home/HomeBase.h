#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/WalkerRates.h"
#include "Walker/Crew/CrewManager.h"
#include "Walker/Crew/WalkerTech.h"
#include "Walker/Home/Garage.h"
#include "Walker/Home/TimeBank.h"
#include "Walker/Home/Wallet.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Journey/Journey.h"
#include "Walker/Milestones/Milestones.h"
#include "Walker/Milestones/WalkerStats.h"
#include "Walker/Travel/Vehicle.h"

#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>
#include <Utilities/Handle.h>

#include <memory>
#include <optional>
#include <span>
#include <vector>

namespace Walker {
	struct HomeBase {
		HomeBase();

		TechManager Tech{};
		MilestoneManager Milestones{};
		WalkerRates Rates;
		CrewManager Crew;
		TimeBank OfflineTime;

		Wallet Funds{};
		Garage Vehicles{};
		StatManager Stats{};

		Journey& GetJourney() { return m_Journey; }
		const Journey& GetJourney() const { return m_Journey; }

		EndpointKind FurthestEndpoint{EndpointKind::Unset};
		EndpointKind GetMaxScoutKind() const;

		bool TryHireCrew(u64 count = 1);

		void Tick(BaseTime elapsed);

		bool TryRebirth();
		bool TryPrestige();
		bool TryAscend();

	private:
		std::vector<ScopedHandle> m_Subs{};
		Journey m_Journey;

		void RebirthReset();
		void PrestigeReset();
		void AscendReset();
	};
}