#pragma once
#include "Reward.h"

#include "Platform/NumTypes.h"

namespace TechMaze {
	struct Breakthrough {
        Breakthrough(u64 pos, Reward reward) : Pos(pos), Reward(reward){}

		u64 Pos{0};
        Reward Reward{Reward::Speed};
	};
}