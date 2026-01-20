#pragma once

#include "Constexpr/ConstexprStrUtils.h"

#include <string>
#include <vector>

namespace _TypeImpl {
    template<typename T>
    static constexpr std::string_view Name() {
#ifdef _MSC_VER
        constexpr std::string_view sig = __FUNCSIG__;
        constexpr std::string_view key = "TypeNameView<";
        constexpr std::string_view end = ">";
#else
        constexpr std::string_view sig = __PRETTY_FUNCTION__;
        constexpr std::string_view key = "T = ";
        constexpr std::string_view end = "]";
#endif
        auto start = sig.find(key);
        if(start == std::string_view::npos) return sig;
        start += key.size();
        auto finish = sig.rfind(end);
        if(finish == std::string_view::npos) finish = sig.size();
        return sig.substr(start, finish - start);
    }
}
namespace Type {
    using TypeId = size_t;
    inline TypeId maxIndex = 0;

    template<typename T>
    TypeId Id() {
        static TypeId index = maxIndex++;
        return index;
    }

    template<typename T>
    static std::string Name() {
        std::string s{_TypeImpl::Name<T>()};
        static std::vector<std::string> toRemove = {"struct", "class", "enum", "union"};
        for(const auto& token: toRemove) {
            s = Constexpr::RemoveAll(s, token);
        }
        s.erase(std::remove_if(s.begin(), s.end(), [](auto c) { return std::isspace(c); }), s.end());
        return s;
    }
} // namespace Type