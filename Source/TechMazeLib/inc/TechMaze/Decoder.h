#pragma once
#include "TechMaze/Technology.h"
#include "Platform/NumTypes.h"
#include <compare>

namespace TechMaze {
	struct Target {
        Communication CommEra : 4{Communication::Verbal};
        Military MilEra : 4{Military::Militia};
        Settlement SettleEra : 4{Settlement::Hamlet};
        Technology TechEra : 4{Technology::Stone};
        Transportation TransEra : 4{Transportation::Foot};
        u8 CommLevel{0};
        u8 MilLevel{0};
        u8 SettleLevel{0};
        u8 TechLevel{0};
        u8 TransLevel{0};

        constexpr auto operator<=>(const Target& other) const noexcept = default;
	};

    constexpr Target ToTarget(u64 encoded) {
        Target result{};
        size_t currentBit = 0;
        // eras
        result.CommEra = static_cast<Communication>((encoded >> currentBit) & 0xF);
        currentBit += 4;
        result.MilEra = static_cast<Military>((encoded >> currentBit) & 0xF);
        currentBit += 4;
        result.SettleEra = static_cast<Settlement>((encoded >> currentBit) & 0xF);
        currentBit += 4;
        result.TechEra = static_cast<Technology>((encoded >> currentBit) & 0xF);
        currentBit += 4;
        result.TransEra = static_cast<Transportation>((encoded >> currentBit) & 0xF);
        currentBit += 4;
        // levels
        result.CommLevel = static_cast<u8>((encoded >> currentBit) & 0xFF);
        currentBit += 8;
        result.MilLevel = static_cast<u8>((encoded >> currentBit) & 0xFF);
        currentBit += 8;
        result.SettleLevel = static_cast<u8>((encoded >> currentBit) & 0xFF);
        currentBit += 8;
        result.TechLevel = static_cast<u8>((encoded >> currentBit) & 0xFF);
        currentBit += 8;
        result.TransLevel = static_cast<u8>((encoded >> currentBit) & 0xFF);

        return result;
    }
    constexpr u64 FromTarget(const Target& target) {
        u64 result = 0;
        size_t currentBit = 0;
        // eras
        result |= static_cast<u64>(target.CommEra) << currentBit;
        currentBit += 4;
        result |= static_cast<u64>(target.MilEra) << currentBit;
        currentBit += 4;
        result |= static_cast<u64>(target.SettleEra) << currentBit;
        currentBit += 4;
        result |= static_cast<u64>(target.TechEra) << currentBit;
        currentBit += 4;
        result |= static_cast<u64>(target.TransEra) << currentBit;
        currentBit += 4;
        // levels
        result |= static_cast<u64>(target.CommLevel) << currentBit;
        currentBit += 8;
        result |= static_cast<u64>(target.MilLevel) << currentBit;
        currentBit += 8;
        result |= static_cast<u64>(target.SettleLevel) << currentBit;
        currentBit += 8;
        result |= static_cast<u64>(target.TechLevel) << currentBit;
        currentBit += 8;
        result |= static_cast<u64>(target.TransLevel) << currentBit;
        return result;
    }
} // namespace TechMaze