#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Utilities/EnumUtils.h>

namespace Walker {
	HomeBase::HomeBase() 
		: Rates(Tech, Milestones)
		, Crew(Rates, Tech)
		, OfflineTime(Rates)
		, m_Journey(*this) {
		ServiceLocator::Get().GetRequired<PubSub<PhaseChanged>>().Subscribe(m_Subs, [this](const PhaseChanged& change) {
			using enum Phase;
			using enum MilestoneTriggerEvent;
			auto Send = [&](MilestoneTriggerEvent trigger) { Milestones.OnEvent(trigger, *this, &m_Journey); };

			if (change.To == Loading) {
				Send(EndpointReached);
			}
			else if (change.From == Unloading) {
				Send(RoundTripComplete);
				if (change.To == Complete) Send(EndpointComplete);
			}
		});

		RebirthReset();
	}

	void HomeBase::Tick(BaseTime elapsed) {
		m_Journey.Tick(elapsed);
		Crew.Tick(elapsed);
	}

	EndpointKind HomeBase::GetMaxScoutKind() const {
		return Enum::Increment(FurthestEndpoint);
	}

	bool HomeBase::TryHireCrew(u64 count) {
		auto cost = Rates.GetHireCost(Crew.GetHiredCount(), count);
		if(Funds.Spend(cost)) {
			Crew.Hire(count);
			Stats.OnCrewHired(count);
			return true;
		}

		return false;
	}

	bool HomeBase::TryRebirth() {
		auto points = Rates.GetRebirthReward(Stats.SinceRebirth().TotalDistance);
		if(points == 0) return false;
		
		Rates.GetRebirth().AddPoints(points);
		RebirthReset();
		Stats.OnRebirth();
		return true;
	}

	bool HomeBase::TryPrestige() {
		auto points = Rates.GetPrestigeReward(Stats.SincePrestige().TotalDistance);
		if(points == 0) return false;

		Rates.GetPrestige().AddPoints(points);
		PrestigeReset();
		Stats.OnPrestige();
		return true;
	}

	bool HomeBase::TryAscend() {
		auto points = Rates.GetAscendReward(Stats.SinceAscend().TotalDistance);
		if (points == 0) return false;

		Rates.GetAscend().AddPoints(points);
		AscendReset();
		Stats.OnAscend();
		return true;
	}

	void HomeBase::RebirthReset() {
		m_Journey.Reset();
		FurthestEndpoint = EndpointKind::Unset;
		Funds.Reset();
		Vehicles.Reset();
		m_Journey.ChangeVehicle(*Vehicles.GetSelected());
		Crew.RebirthReset();
		Stats.RebirthReset();
	}

	void HomeBase::PrestigeReset() {
		Tech.Reset();
		Crew.PrestigeReset();
		Rates.PrestigeReset();
		Stats.PrestigeReset();
		RebirthReset();
	}

	void HomeBase::AscendReset() {
		Rates.AscendReset();
		Stats.AscendReset();
		PrestigeReset();
	}
}