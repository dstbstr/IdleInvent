#pragma once

#include "GameState/GameTime.h"
#include <variant>
#include <functional>
#include <type_traits>

class Accumulator {
    using Callback = std::function<void()>;
    using CtxCallback = std::function<void(void*)>;
    BaseTime m_Interval{0};
    BaseTime m_Time{0};
    std::variant<Callback, CtxCallback> m_Callback;

public:
    Accumulator(BaseTime interval, Callback callback) 
        : m_Interval(interval)
        , m_Callback(std::move(callback)) {};
    Accumulator(BaseTime interval, CtxCallback callback)
        : m_Interval(interval)
        , m_Callback(std::move(callback)) {};

    void Tick(BaseTime elapsed, void* context = nullptr) {
        m_Time += elapsed;
        while (m_Time >= m_Interval) {
            m_Time -= m_Interval;
            std::visit([context](auto& cb) {
                using T = std::decay_t<decltype(cb)>;
                if constexpr(std::is_same_v<T, Callback>) {
                    cb();
                } else if constexpr(std::is_same_v<T, CtxCallback>) {
                    cb(context);
                }
            }, m_Callback);
        }
    }
};