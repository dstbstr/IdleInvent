#include "InventLib/Character/RebirthBenefits.h"

#include <unordered_map>

namespace {
    using namespace Invent;
    std::unordered_map<std::string, Effect> benefits{
        {"Worker Density", Effect{.Target = EffectTarget::WorkerDensity, .Modifier = {.Add = -1, .Mul = 1.0f}}},
        {"Project Efficiency", Effect{.Target = EffectTarget::AllProjectEfficiency, .Modifier = {.Add = 1, .Mul = 1.0F}}},
        {"Project Rate", Effect{.Target = EffectTarget::AllProjectRate, .Modifier = {.Add = 1, .Mul = 1.0F}}},
		{"Extra Time Shards", Effect{.Target = EffectTarget::TimeShardCap, .Modifier = {.Add = 1, .Mul = 1.0F}}},
		{"Tick Bonus", Effect{.Target = EffectTarget::TickPower, .Modifier = {.Add = 1, .Mul = 1.0F}}},
		{"Conversion Bonus", Effect{.Target = EffectTarget::ConversionPower, .Modifier = {.Add = 1, .Mul = 1.0F}}},
        {"Exploration Success Bonus", Effect{.Target = EffectTarget::ExploreSuccessRate, .Modifier = {.Add = 0, .Mul = 1.1F}}}
    };
}

namespace Invent {
    std::unordered_map<std::string, Effect> GetAllRebirthBenefits() { return benefits; }
}
	