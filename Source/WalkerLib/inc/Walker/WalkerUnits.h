#pragma once

#include <Math/BigInt.h>

namespace Walker {
	using Quantity = BigIntImpl<40, 23, true>;

	using Distance = Quantity; // millimeters
    using Speed = Quantity; // Distance per second
    using Acceleration = Quantity; // Speed per second
    using CargoAmount = Quantity; // milligrams
    using Work = Quantity; 
    using WorkRate = Quantity; // Work per second
    using FuelEfficiency = Quantity; // Milliseconds per Kg

    namespace Literals {
        inline constexpr auto LightYear = Distance::FromScientific(9'460'730'472'580'800ull, 3);

        constexpr Distance operator""_m(u64 val) { return Distance::FromScientific(val, 3); }
        constexpr Distance operator""_Km(u64 val) { return Distance::FromScientific(val, 6); }
        constexpr Distance operator""_Mm(u64 val) { return Distance::FromScientific(val, 9); }
        constexpr Distance operator""_Gm(u64 val) { return Distance::FromScientific(val, 12); }
        constexpr Distance operator""_Ly(u64 val) { return Distance(val) * LightYear; }
        constexpr Distance operator""_KLy(u64 val) { return Distance::FromScientific(val, 3) * LightYear; }
        constexpr Distance operator""_MLy(u64 val) { return Distance::FromScientific(val, 6) * LightYear; }
        constexpr Distance operator""_GLy(u64 val) { return Distance::FromScientific(val, 9) * LightYear; }

        constexpr CargoAmount operator""_g(u64 val) { return CargoAmount::FromScientific(val, 3); }
        constexpr CargoAmount operator""_Kg(u64 val) { return CargoAmount::FromScientific(val, 6); }
        constexpr CargoAmount operator""_Mg(u64 val) { return CargoAmount::FromScientific(val, 9); }
        constexpr CargoAmount operator""_Gg(u64 val) { return CargoAmount::FromScientific(val, 12); }
        constexpr CargoAmount operator""_t(u64 val) { return CargoAmount::FromScientific(val, 9); }
        constexpr CargoAmount operator""_Kt(u64 val) { return CargoAmount::FromScientific(val, 12); }
        constexpr CargoAmount operator""_Mt(u64 val) { return CargoAmount::FromScientific(val, 15); }
        constexpr CargoAmount operator""_Gt(u64 val) { return CargoAmount::FromScientific(val, 18); }

        constexpr Work operator""_j(u64 val) { return Work::FromScientific(val, 3); }
        constexpr Work operator""_Kj(u64 val) { return Work::FromScientific(val, 6); }
        constexpr Work operator""_Mj(u64 val) { return Work::FromScientific(val, 9); }
        constexpr Work operator""_Gj(u64 val) { return Work::FromScientific(val, 12); }

        constexpr WorkRate operator""_w(u64 val) { return WorkRate::FromScientific(val, 3); }
        constexpr WorkRate operator""_Kw(u64 val) { return WorkRate::FromScientific(val, 6); }
        constexpr WorkRate operator""_Mw(u64 val) { return WorkRate::FromScientific(val, 9); }
        constexpr WorkRate operator""_Gw(u64 val) { return WorkRate::FromScientific(val, 12); }

        constexpr Speed operator""_mps(u64 val) { return Speed::FromScientific(val, 3); }
        constexpr Speed operator""_Kmps(u64 val) { return Speed::FromScientific(val, 6); }
        constexpr Speed operator""_Mmps(u64 val) { return Speed::FromScientific(val, 9); }
        constexpr Speed operator""_Gmps(u64 val) { return Speed::FromScientific(val, 12); }
        constexpr Speed operator""_Lyps(u64 val) { return Speed(val) * LightYear; }
        constexpr Speed operator""_KLyps(u64 val) { return Speed::FromScientific(val, 3) * LightYear; }
        constexpr Speed operator""_MLyps(u64 val) { return Speed::FromScientific(val, 6) * LightYear; }
        constexpr Speed operator""_GLyps(u64 val) { return Speed::FromScientific(val, 9) * LightYear; }

        constexpr Acceleration operator""_mps2(u64 val) { return Acceleration::FromScientific(val, 3); }
        constexpr Acceleration operator""_Kmps2(u64 val) { return Acceleration::FromScientific(val, 6); }
        constexpr Acceleration operator""_Mmps2(u64 val) { return Acceleration::FromScientific(val, 9); }
        constexpr Acceleration operator""_Gmps2(u64 val) { return Acceleration::FromScientific(val, 12); }
        constexpr Acceleration operator""_Lyps2(u64 val) { return Acceleration(val) * LightYear; }
        constexpr Acceleration operator""_KLyps2(u64 val) { return Acceleration::FromScientific(val, 3) * LightYear; }
        constexpr Acceleration operator""_MLyps2(u64 val) { return Acceleration::FromScientific(val, 6) * LightYear; }

        constexpr FuelEfficiency operator""_spKg(u64 val) { return FuelEfficiency::FromScientific(val, 3); }
        constexpr FuelEfficiency operator""_mpKg(u64 val) { return FuelEfficiency::FromScientific(val, 3) * 60; }
        constexpr FuelEfficiency operator""_hpKg(u64 val) { return FuelEfficiency::FromScientific(val, 3) * 3600; }
        constexpr FuelEfficiency operator""_dpKg(u64 val) { return FuelEfficiency::FromScientific(val, 3) * 3600 * 24; }
    }
}