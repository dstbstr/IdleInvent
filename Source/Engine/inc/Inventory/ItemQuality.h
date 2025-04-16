#pragma once

#include <string>

namespace Invent {
    enum struct QualityPrefix {
        Cursed,
        WornOut,
        SlightlyUsed,
        BrandNew,
        NewlyInvented,
        Magical,
        FIRST = Cursed,
        LAST = Magical
    };
    enum struct QualityGeneral {
        Poor,
        Common,
        Good,
        Excellent,
        Masterwork,
        Legendary,
        Ultimate,
        FIRST = Poor,
        LAST = Ultimate
    };
    enum struct QualityMaterial {
        Wooden,
        Stone,
        Iron,
        Steel,
        Mithril,
        Adamantium,
        Alien,
        FIRST = Wooden,
        LAST = Alien
    };
    enum struct QualityPostFix {
        IThink,
        AccordingToVern,
        None,
        ButBetter,
        FromHeaven,
        OfDoom,
        FIRST = IThink,
        LAST = OfDoom
    };

    constexpr std::string ToString(QualityPrefix prefix) {
        switch(prefix) {
        case QualityPrefix::Cursed: return "Cursed";
        case QualityPrefix::WornOut: return "Worn Out";
        case QualityPrefix::SlightlyUsed: return "Slightly Used";
        case QualityPrefix::BrandNew: return "Brand New";
        case QualityPrefix::NewlyInvented: return "Newly Invented";
        case QualityPrefix::Magical: return "Magical";
        }
        return "Unknown";
    }

    constexpr std::string ToString(QualityGeneral quality) {
        switch(quality) {
        case QualityGeneral::Poor: return "Poor";
        case QualityGeneral::Common: return "Common";
        case QualityGeneral::Good: return "Good";
        case QualityGeneral::Excellent: return "Excellent";
        case QualityGeneral::Masterwork: return "Masterwork";
        case QualityGeneral::Legendary: return "Legendary";
        case QualityGeneral::Ultimate: return "Ultimate";
        }
        return "Unknown";
    }

    constexpr std::string ToString(QualityMaterial material) {
        switch(material) {
        case QualityMaterial::Wooden: return "Wooden";
        case QualityMaterial::Stone: return "Stone";
        case QualityMaterial::Iron: return "Iron";
        case QualityMaterial::Steel: return "Steel";
        case QualityMaterial::Mithril: return "Mithril";
        case QualityMaterial::Adamantium: return "Adamantium";
        case QualityMaterial::Alien: return "Alien";
        }
        return "Unknown";
    }

    constexpr std::string ToString(QualityPostFix postFix) {
        switch(postFix) {
        case QualityPostFix::IThink: return "I think?";
        case QualityPostFix::AccordingToVern: return "According to Vern";
        case QualityPostFix::None: return "";
        case QualityPostFix::ButBetter: return "But better";
        case QualityPostFix::FromHeaven: return "From Heaven";
        case QualityPostFix::OfDoom: return "Of Doom!";
        }
        return "Unknown";
    }

    struct ItemQuality {
        QualityPrefix Prefix{QualityPrefix::FIRST};
        QualityGeneral General{QualityGeneral::FIRST};
        QualityMaterial Material{QualityMaterial::FIRST};
        QualityPostFix PostFix{QualityPostFix::FIRST};

        u8 GetQuality() const {
            return static_cast<u8>(Prefix) + static_cast<u8>(General) + static_cast<u8>(Material) +
                   static_cast<u8>(PostFix);
        }

        std::string GetDescription(const std::string& itemName) const {
            return std::format(
                "{} {} {} {} {}", ToString(Prefix), ToString(General), ToString(Material), itemName, ToString(PostFix)
            );
        }
    };

} // namespace Invent