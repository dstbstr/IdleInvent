#include "TechMaze/Decoder.h"

namespace TechMaze {
	constexpr bool TestDecorder() {
        Target original{
            .CommEra = Communication::Verbal,
            .MilEra = Military::Cavalry,
            .SettleEra = Settlement::City,
            .TechEra = Technology::Steel,
            .TransEra = Transportation::Plane,
            .CommLevel = 1,
            .MilLevel = 2,
            .SettleLevel = 3,
            .TechLevel = 4,
            .TransLevel = 5
        };

        u64 encoded = FromTarget(original);
        Target decoded = ToTarget(encoded);
        return decoded == original;
	}

    static_assert(TestDecorder(), "Decoder test failed");

	/*
	    enum struct Communication : u8 { Verbal, Written, Printed, Digital, Global, Telepathic };
    enum struct Military : u8 { Militia, Cavalry, Platoon, Regiment, Battalion, ArmedForces };
    enum struct Settlement : u8 { Hamlet, Town, City, Metro, Kingdom, Empire };
    enum struct Technology : u8 { Stone, Bronze, Iron, Steel, Electronic, Nano };
    enum struct Transportation : u8 { Foot, Horse, Carriage, Ship, Plane, Rocket };

	*/
}