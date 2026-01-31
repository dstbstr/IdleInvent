#pragma once

#include "GameState/GameTime.h"

class Accumulator {
    using Callback = std::function<void()>;
    BaseTime m_Interval{0};
    BaseTime m_Time{0};
    Callback m_Callback;

public:
    Accumulator(BaseTime interval, Callback callback) : m_Interval(interval), m_Callback(callback) {};

    void Tick(BaseTime elapsed) {
        m_Time += elapsed;
        while (m_Time >= m_Interval) {
            m_Time -= m_Interval;
            m_Callback();
        }
    }
};