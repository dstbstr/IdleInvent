#pragma once

#include "GhostHunter/Quality/Quality.h"

#include "Resources/Resource.h"
#include <Platform/NumTypes.h>

#include <string>
#include <vector>

namespace GhostHunter {
    enum struct MediaType : u8 {
        Unset,

        Picture,
        Interview,
        Article,
        Book,
        Podcast,
        TvShow,
        Movie,

        COUNT
    };

    std::string ToString(MediaType type);

    struct Media {
        MediaType Type{MediaType::Unset};
        QualityType Quality{0};
    };

    ResourceCollection UpgradeCost(const Media& media);
}