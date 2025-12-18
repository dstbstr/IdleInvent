#pragma once

#include <Platform/NumTypes.h>

namespace GhostHunter {
    enum struct Media : u8 {
        Unset,

        FaireBooth,
        Interview,
        Article,
        Book,
        Podcast,
        TvShow,
        Movie
    };
}