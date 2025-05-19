#pragma once

#include "TechMaze/Technology.h"

#include <format>
#include <ostream>

template<>
struct std::formatter<TechMaze::Communication> : std::formatter<std::string> {
    auto format(const TechMaze::Communication& input, std::format_context& ctx) const {
        std::string name;
        switch(input) {
            using enum TechMaze::Communication;
        case Verbal: name = "Verbal"; break;
        case Written: name = "Written"; break;
        case Printed: name = "Printed"; break;
        case Digital: name = "Digital"; break;
        case Global: name = "Global"; break;
        case Telepathic: name = "Telepathic"; break;
        default: name = "Unknown"; break;
        }
        return std::formatter<std::string>::format(std::format("{}", name), ctx);
    }
};

template<>
struct std::formatter<TechMaze::Military> : std::formatter<std::string> {
    auto format(const TechMaze::Military& input, std::format_context& ctx) const {
        std::string name;
        switch(input) {
            using enum TechMaze::Military;
        case Militia: name = "Militia"; break;
        case Cavalry: name = "Cavalry"; break;
        case Platoon: name = "Platoon"; break;
        case Regiment: name = "Regiment"; break;
        case Battalion: name = "Battalion"; break;
        case ArmedForces: name = "Armed Forces"; break;
        default: name = "Unknown"; break;
        }
        return std::formatter<std::string>::format(std::format("{}", name), ctx);
    }
};

template<>
struct std::formatter<TechMaze::Settlement> : std::formatter<std::string> {
    auto format(const TechMaze::Settlement& input, std::format_context& ctx) const {
        std::string name;
        switch(input) {
            using enum TechMaze::Settlement;
        case Hamlet: name = "Hamlet"; break;
        case Town: name = "Town"; break;
        case City: name = "City"; break;
        case Metro: name = "Metro"; break;
        case Kingdom: name = "Kingdom"; break;
        case Empire: name = "Empire"; break;
        default: name = "Unknown"; break;
        }
        return std::formatter<std::string>::format(std::format("{}", name), ctx);
    }
};

template<>
struct std::formatter<TechMaze::Technology> : std::formatter<std::string> {
    auto format(const TechMaze::Technology& input, std::format_context& ctx) const {
        std::string name;
        switch(input) {
            using enum TechMaze::Technology;
        case Stone: name = "Stone"; break;
        case Bronze: name = "Bronze"; break;
        case Iron: name = "Iron"; break;
        case Steel: name = "Steel"; break;
        case Electronic: name = "Electronic"; break;
        case Nano: name = "Nano"; break;
        default: name = "Unknown"; break;
        }
        return std::formatter<std::string>::format(std::format("{}", name), ctx);
    }
};

template<>
struct std::formatter<TechMaze::Transportation> : std::formatter<std::string> {
    auto format(const TechMaze::Transportation& input, std::format_context& ctx) const {
        std::string name;
        switch(input) {
            using enum TechMaze::Transportation;
        case Foot: name = "Foot"; break;
        case Horse: name = "Horse"; break;
        case Carriage: name = "Carriage"; break;
        case Ship: name = "Ship"; break;
        case Plane: name = "Plane"; break;
        case Rocket: name = "Rocket"; break;
        default: name = "Unknown"; break;
        }
        return std::formatter<std::string>::format(std::format("{}", name), ctx);
    }
};

template<>
struct std::formatter<TechMaze::Power> : std::formatter<std::string> {
    auto format(const TechMaze::Power& input, std::format_context& ctx) const {
        std::string name;
        switch(input) {
            using enum TechMaze::Power;
        case Human: name = "Human"; break;
        case Animal: name = "Animal"; break;
        case Steam: name = "Steam"; break;
        case Combustion: name = "Combustion"; break;
        case Fission: name = "Fission"; break;
        case Fusion: name = "Fusion"; break;
        default: name = "Unknown"; break;
        }
        return std::formatter<std::string>::format(std::format("{}", name), ctx);
    }
};

inline std::ostream& operator<<(std::ostream& os, const TechMaze::Communication& e) {
    os << std::format("{}", e);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const TechMaze::Military& e) {
    os << std::format("{}", e);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const TechMaze::Settlement& e) {
    os << std::format("{}", e);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const TechMaze::Technology& e) {
    os << std::format("{}", e);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const TechMaze::Transportation& e) {
    os << std::format("{}", e);
    return os;
}
