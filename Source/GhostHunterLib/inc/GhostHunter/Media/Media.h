#pragma once

#include "GhostHunter/Quality/Quality.h"

#include "Resources/Resource.h"

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
        using IdType = MediaType;
        using LevelType = QualityType;

        MediaType Id{MediaType::Unset};
        QualityType Level{0};

        std::string Describe() const;
        void OnUpgrade();
    };
}