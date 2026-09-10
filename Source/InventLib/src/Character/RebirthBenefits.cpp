#include "Invent/Character/RebirthBenefits.h"

#include <unordered_map>

namespace {
    using namespace Invent;
    std::unordered_map<std::string, Effect> benefits{
        {"Worker Density", Effect{.Target = EffectTarget::WorkerDensity, .Mod = {.Add = -1, .Mul = 1.0f}}},
        {"Project Efficiency", Effect{.Target = EffectTarget::AllProjectEfficiency, .Mod = {.Add = 1, .Mul = 1.0F}}},
        {"Project Rate", Effect{.Target = EffectTarget::AllProjectRate, .Mod = {.Add = 1, .Mul = 1.0F}}},
		{"Extra Time Shards", Effect{.Target = EffectTarget::TimeShardCap, .Mod = {.Add = 1, .Mul = 1.0F}}},
		{"Tick Bonus", Effect{.Target = EffectTarget::TickPower, .Mod = {.Add = 1, .Mul = 1.0F}}},
		{"Conversion Bonus", Effect{.Target = EffectTarget::ConversionPower, .Mod = {.Add = 1, .Mul = 1.0F}}},
        {"Exploration Success Bonus", Effect{.Target = EffectTarget::ExploreSuccessRate, .Mod = {.Add = 0, .Mul = 1.1F}}}
    };
}

namespace Invent {
    std::unordered_map<std::string, Effect> GetAllRebirthBenefits() { return benefits; }
}
	