#pragma once

#include <GameState/GameTime.h>
#include <Math/BigInt.h>

namespace Walker {
	using Quantity = BigIntImpl<40, 23, true>;
    static constexpr Quantity Zero{0};

	using Distance = Quantity; // millimeters
    using Speed = Quantity; // Distance per second
    using Acceleration = Quantity; // Speed per second
    using Mass = Quantity; // milligrams
    using Work = Quantity; 
    using WorkRate = Quantity; // Work per second
    using FuelEfficiency = Quantity; // Milliseconds per Kg
    using Time = Quantity; // Millseconds
    using Money = Quantity; // millicopper pieces

    static constexpr BaseTime UpdateInterval = OneSecond;
    static constexpr Quantity StepMs{UpdateInterval.count()};
    static constexpr Quantity MsPerSec{OneSecond.count()};

	constexpr BaseTime ToBaseTime(Quantity q) { return OneInstant * static_cast<BaseTime::rep>(Time::Ratio(q, Time{ 1 })); }
	constexpr Quantity ToWalkerTime(BaseTime t) { return Quantity{ t.count() }; }

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

        constexpr Mass operator""_g(u64 val) { return Mass::FromScientific(val, 3); }
        constexpr Mass operator""_Kg(u64 val) { return Mass::FromScientific(val, 6); }
        constexpr Mass operator""_Mg(u64 val) { return Mass::FromScientific(val, 9); }
        constexpr Mass operator""_Gg(u64 val) { return Mass::FromScientific(val, 12); }
        constexpr Mass operator""_t(u64 val) { return Mass::FromScientific(val, 9); }
        constexpr Mass operator""_Kt(u64 val) { return Mass::FromScientific(val, 12); }
        constexpr Mass operator""_Mt(u64 val) { return Mass::FromScientific(val, 15); }
        constexpr Mass operator""_Gt(u64 val) { return Mass::FromScientific(val, 18); }

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

        constexpr Time operator""_ms(u64 val) { return Time::FromScientific(val, 0); }
        constexpr Time operator""_sec(u64 val) { return Time::FromScientific(val, 3); }
        constexpr Time operator""_min(u64 val) { return Time::FromScientific(val, 3) * 60; }
        constexpr Time operator""_hour(u64 val) { return Time::FromScientific(val, 3) * 3'600; }
        constexpr Time operator""_day(u64 val) { return Time::FromScientific(val, 3) * 3'600 * 24; }

        constexpr Money operator""_cp(u64 val) { return Money::FromScientific(val, 3); }
        constexpr Money operator""_sp(u64 val) { return Money::FromScientific(val, 5); }
        constexpr Money operator""_gp(u64 val) { return Money::FromScientific(val, 7); }
        constexpr Money operator""_pp(u64 val) { return Money::FromScientific(val, 9); }

    }
}