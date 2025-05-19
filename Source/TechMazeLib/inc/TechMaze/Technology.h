#pragma once
#include "Platform/NumTypes.h"

namespace TechMaze {
    enum struct Communication : u8 { Verbal, Written, Printed, Digital, Global, Telepathic };
    enum struct Military : u8 { Militia, Cavalry, Platoon, Regiment, Battalion, ArmedForces };
    enum struct Settlement : u8 { Hamlet, Town, City, Metro, Kingdom, Empire };
    enum struct Technology : u8 { Stone, Bronze, Iron, Steel, Electronic, Nano };
    enum struct Transportation : u8 { Foot, Horse, Carriage, Ship, Plane, Rocket };
    enum struct Power : u8 { Human, Animal, Steam, Combustion, Fission, Fusion};
}
