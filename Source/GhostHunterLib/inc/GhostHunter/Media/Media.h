#pragma once

#include "GhostHunter/Quality/Quality.h"

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
        Movie
    };

    struct Media {
        MediaType Type{MediaType::Unset};
        u32 Value{0};
        Media() = default;
        Media(MediaType type, u32 value) : Type(type), Value(value) {}
    };
}