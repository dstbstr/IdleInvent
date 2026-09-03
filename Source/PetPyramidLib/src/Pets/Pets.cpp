#include "Pets/Pets/Pets.h"

#include <ranges>

namespace Pets {
    std::string_view ToString(PetKind kind) {
        switch(kind) {
            using enum PetKind;
        case Bloodhound: return "Bloodhound";
        case Labrador: return "Labrador";
        case Poodle: return "Poodle";
        case Chihuahua: return "Chihuahua";
        case HouseCat: return "House cat";
        case Sabertooth: return "Sabertooth";
        case Lion: return "Lion";
        case Tiger: return "Tiger";
        case Leapord: return "Leapord";
        case Robin: return "Robin";
        case Parrot: return "Parrot";
        case Stork: return "Stork";
        case Pelacin: return "Pelacin";
        case Ostrich: return "Ostrich";
        case Penguin: return "Penguin";
        case Raccoon: return "Raccoon";
        case Rat: return "Rat";
        case Squirel: return "Squirel";
        case Hamster: return "Hamster";
        case Ferret: return "Ferret";
        case Cactus: return "Cactus";
        case Petunias: return "Petunias";
        case VenusFlytrap: return "Venus Flytrap";
        case MonkeyPuzzleTree: return "Monkey Puzzle Tree";
        case Bonzai: return "Bonzai";
        case SanboxTree: return "SanboxTree";
        case Seaweed: return "Seaweed";
        case Pig: return "Pig";
        case Cow: return "Cow";
        case Chicken: return "Chicken";
        case Sheep: return "Sheep";
        case Goat: return "Goat";
        case Rabbit: return "Rabbit";
        case RedHerring: return "Red herring";
        case GoldFish: return "Goldfish";
        case Octopus: return "Octopus";
        case StarFish: return "Starfish";
        case SeaHorse: return "Sea Horse";
        case Anenome: return "Anenome";
        case Shrimp: return "Shrimp";
        case Crab: return "Crab";
        case Clam: return "Clam";
        case Whale: return "Whale";
        case Shark: return "Shark";
        case LadyBug: return "Lady bug";
        case Centipede: return "Centipede";
        case Spider: return "Spider";
        case Scorpion: return "Scorpion";
        case Mantis: return "Mantis";
        case DragonFly: return "Dragonfly";
        case Dragon: return "Dragon";
        case WorldTree: return "World Tree";
        case Fairy: return "Fairy";
        case Goblin: return "Goblin";
        case Ghost: return "Ghost";
        case Phoenix: return "Phoenix";
        case GarbageCan: return "Garbage Can";
        case Lightbulb: return "Lightbulb";
        case Plate: return "Plate";
        case Book: return "Book";
        case Robot: return "Robot";
        case Teapot: return "Teapot";
        case MoneyBag: return "Money Bag";
        case Car: return "Car";
        case Motorcycle: return "Motorcycle";
        case Truck: return "Truck";
        case Bus: return "Bus";
        case Plane: return "Plane";
        case Train: return "Train";
        case Bike: return "Bike";
        case Skateboard: return "Skateboard";
        case Ship: return "Ship";
        case Boat: return "Boat";
        case Turtle: return "Turtle";
        case Snake: return "Snake";
        case Lizard: return "Lizard";
        case Salamander: return "Salamander";
        case Toad: return "Toad";
        }
        return "Unknown";
    }

}