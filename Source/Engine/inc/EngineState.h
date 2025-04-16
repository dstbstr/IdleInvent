#pragma once

#include <chrono>

struct EngineState {
    std::chrono::milliseconds ElapsedTime;
    size_t FrameNum;
};