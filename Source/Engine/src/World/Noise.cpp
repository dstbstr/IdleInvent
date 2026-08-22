#include "World/Noise.h"

static_assert(Smoothstep::Cubic(0.0) == 0.0);
static_assert(Smoothstep::Cubic(0.5) == 0.5);
static_assert(Smoothstep::Cubic(1.0) == 1.0);
static_assert(Smoothstep::Cubic(0.25) < Smoothstep::Cubic(0.3));
static_assert(Smoothstep::Cubic(0.75) < Smoothstep::Cubic(0.8));

static_assert(Smoothstep::Quintic(0.0) == 0.0);
static_assert(Smoothstep::Quintic(0.5) == 0.5);
static_assert(Smoothstep::Quintic(1.0) == 1.0);
static_assert(Smoothstep::Quintic(0.25) < Smoothstep::Quintic(0.3));
static_assert(Smoothstep::Quintic(0.75) < Smoothstep::Quintic(0.8));

static_assert(Noise::MixSeed(0) == 0);
static_assert(Noise::MixSeed(1) != 1);
static_assert(Noise::MixSeed(1) != Noise::MixSeed(2));