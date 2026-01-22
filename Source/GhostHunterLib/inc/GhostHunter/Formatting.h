#pragma once

#include "Investigation/Evidence.h"
#include "Locations/Locations.h"
#include "Media/Media.h"
#include "Resources/GhostHunterResources.h"
#include "GhostHunter/Quality/Quality.h"
#include "Tools/Tools.h"

#include <format>
#include <ostream>
#include <string>

namespace GhostHunter {
    std::string ToString(LocationName location);
    std::string ToString(MediaType type);
    std::string ToString(ResourceName name);
    std::string ToString(QualityType quality);
    std::string ToString(ToolName tool);
    std::string ToString(EvidenceType type);
}

#define STREAM_FROM_FORMAT(_Type) \
inline std::ostream& operator<<(std::ostream& os, const GhostHunter::_Type& e) { \
    os << std::format("{}", e); \
    return os; \
}

#define FORMAT_GHOSTHUNTER_ENUM(_EnumType) \
template<> \
struct std::formatter<GhostHunter::_EnumType> : std::formatter<std::string> { \
    auto format(const GhostHunter::_EnumType& input, std::format_context& ctx) const { \
        return std::formatter<std::string>::format(std::format("{}", ToString(input)), ctx); \
    } \
}; \
STREAM_FROM_FORMAT(_EnumType)

FORMAT_GHOSTHUNTER_ENUM(LocationName)
FORMAT_GHOSTHUNTER_ENUM(MediaType)
FORMAT_GHOSTHUNTER_ENUM(ResourceName)
FORMAT_GHOSTHUNTER_ENUM(QualityType)
FORMAT_GHOSTHUNTER_ENUM(ToolName)


template<>
struct std::formatter<GhostHunter::Media> : std::formatter<std::string> {
    auto format(const GhostHunter::Media& input, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("{} ${}", input.Type, input.Value), ctx);
    }
};
STREAM_FROM_FORMAT(Media)

#undef FORMAT_GHOSTHUNTER_ENUM
#undef STREAM_FROM_FORMAT