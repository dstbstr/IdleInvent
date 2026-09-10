#pragma once
#include "TOE/Enemy/Enemy.h"
#include "TOE/Hero/Hero.h"

namespace TOE {
    class Encounter {
        //Player Party 1-4
        std::vector<Hero> party{};

        //Enemies
        std::vector<Enemy> enemies{};

        //Turn Order
        //Determines if something Hits
        //Assigns Damage -> Resistance -> Absorption
        //Updates Player or Enemy

    public:
        Encounter(std::vector<Hero>& party, std::vector<Enemy> enemies);
    };
}
