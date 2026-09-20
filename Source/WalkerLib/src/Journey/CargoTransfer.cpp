#include "Walker/Journey/CargoTransfer.h"

#include <algorithm>
#include <stdexcept>

namespace Walker {
    Mass CargoTransfer::Advance(Work addedWork, Work workPerKg, Mass available) {
        using namespace Walker::Literals;

        if(workPerKg <= Zero) throw std::domain_error("Cargo work must be positive");
        if(addedWork < Zero) throw std::domain_error("Added work cannot be negative");
        if(available <= Zero || Transferred >= Target) return Zero;
        
        AccumulatedWork += addedWork;
        auto earnedMass = AccumulatedWork * 1_Kg / workPerKg;
        auto pendingMass = std::max(Zero, earnedMass - Transferred);
        auto remaining = Target - Transferred;
        auto transferred = std::min(std::min(pendingMass, available), remaining);

        Transferred += transferred;
        return transferred;
    }

    f32 CargoTransfer::GetProgress() const { 
        if(Target <= Zero) return 1.f;

        return std::clamp(static_cast<f32>(Mass::Ratio(Transferred, Target)), 0.f, 1.f); 
    }

    std::optional<Time> CargoTransfer::GetEta(WorkRate rate, Work workPerKg) const {
        using namespace Walker::Literals;

		if (workPerKg <= Zero) throw std::domain_error("Cargo work must be positive");
        if(Transferred >= Target) return Zero;

        auto remainingWork = std::max(Zero, Target * workPerKg - AccumulatedWork * 1_Kg);
        if(remainingWork == Zero) return Zero;
        if(rate <= Zero) return std::nullopt;

        return remainingWork * MsPerSec / (rate * 1_Kg);
    }
}