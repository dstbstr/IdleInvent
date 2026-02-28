#include "TechMaze/Finder.h"
#include "TechMaze/Decoder.h"

namespace {
    TechMaze::FindResult GetFullResult(const TechMaze::Target& current, const TechMaze::Target& expected) {
        using namespace TechMaze;
        using enum TechMaze::EraStatus;
        auto GetStatus = [](const auto& currentEra, const auto& expectedEra, const auto& currentLevel, const auto& expectedLevel ) -> TechMaze::EraStatus {
            if(currentEra < expectedEra) return Low;
            if(currentEra > expectedEra) return High;
            if(currentLevel < expectedLevel) return LowLevel;
            return Correct;
        };

        TechMaze::FindResult result{};
        result.Status[0] = GetStatus(current.CommEra, expected.CommEra, current.CommLevel, expected.CommLevel);
        result.Status[1] = GetStatus(current.MilEra, expected.MilEra, current.MilLevel, expected.MilLevel);
        result.Status[2] = GetStatus(current.SettleEra, expected.SettleEra, current.SettleLevel, expected.SettleLevel);
        result.Status[3] = GetStatus(current.TechEra, expected.TechEra, current.TechLevel, expected.TechLevel);
        result.Status[4] = GetStatus(current.TransEra, expected.TransEra, current.TransLevel, expected.TransLevel);

        result.CountCorrect = static_cast<u8>(std::count(result.Status.begin(), result.Status.end(), TechMaze::EraStatus::Correct));
        result.CountLow = static_cast<u8>(std::count(result.Status.begin(), result.Status.end(), TechMaze::EraStatus::Low));
        result.CountHigh = static_cast<u8>(std::count(result.Status.begin(), result.Status.end(), TechMaze::EraStatus::High));
        return result;
    }

    void CountCorrectOnly(TechMaze::FindResult& result) { 
        result.CountLow = 0;
        result.CountHigh = 0;
        for(auto& status : result.Status) {
            status = TechMaze::EraStatus::Unknown;
        }
    }

    void HighLowCountOnly(TechMaze::FindResult& result) { 
        for(auto& status: result.Status) {
            status = TechMaze::EraStatus::Unknown;
        }
    }

    void HighLowDetail(TechMaze::FindResult& result) {
        for(auto& status : result.Status) {
            if(status == TechMaze::EraStatus::LowLevel) {
                status = TechMaze::EraStatus::Correct;
            }
        }
    }
} // namespace

namespace TechMaze {
    FindResult Finder::Find(u64 currentPos, u64 targetPos) const {
        Target current = ToTarget(currentPos);
        Target target = ToTarget(targetPos);
        FindResult result = GetFullResult(current, target);
        switch(Level) { 
            using enum TechMaze::DetailLevel;
            case CountCorrect: CountCorrectOnly(result); break;
            case CountLowHigh: HighLowCountOnly(result); break;
            case LowHighDetail: HighLowDetail(result); break;
            default: case AllDetail: break;
        }

        return result;
    }
}