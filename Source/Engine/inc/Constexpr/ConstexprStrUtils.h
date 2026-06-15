#pragma once

#include <Platform/NumTypes.h>

#include <array>
#include <charconv>
#include <concepts>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#ifdef __has_include
    #if __has_include(<version>)
        #include <version>
    #endif
#endif

namespace Constexpr {
    constexpr std::string ToString(std::integral auto value) {
        std::array<char, 24> buf{};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto result = std::to_chars(buf.data(), buf.data() + buf.size(), value);
        return std::string(buf.data(), result.ptr);
    }
    constexpr std::string ToString(const char* cstr) { return std::string(cstr); }

    template<typename T>
    constexpr std::optional<T> TryFromString(std::string_view str) {
        T result{};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto res = std::from_chars(str.data(), str.data() + str.size(), result);
        if(res.ec != std::errc()) return std::nullopt;
        return result;
    }
    template<typename T>
    constexpr T FromString(std::string_view str) {
        return *TryFromString<T>(str);
    }

    constexpr std::vector<std::string_view> Split(std::string_view input, std::string_view delimiter, bool keepEmpties = false) {
        size_t last = 0;
        size_t next = 0;
        std::vector<std::string_view> result;

        while((next = input.find(delimiter, last)) != std::string::npos) {
            if(keepEmpties || next - last > 0) {
                result.push_back(input.substr(last, next - last));
            }
            last = next + delimiter.size();
        }

        auto lastEntry = input.substr(last);
        if(lastEntry.length() > 0) {
            result.push_back(lastEntry);
        }
        return result;
    }

    constexpr std::string Join(const std::string& delimeter, const auto& parts) {
#if defined(__cpp_lib_ranges_join_with)
        return parts
            | std::views::transform([](const auto& part) {
                using Raw = std::decay_t<decltype(part)>;
                if constexpr(std::is_same_v<Raw, std::string> || std::is_same_v<std::string_view, Raw>) {
                    return part;
                } else {
                    return ToString(part);
                }
            })
            | std::views::join_with(delimeter) 
            | std::ranges::to<std::string>();
#else
		std::string result;

        for(auto it = std::begin(parts); it != std::end(parts); it++) {
            auto part = *it;
            using Raw = std::decay_t<decltype(part)>;

            if constexpr(std::is_same_v<std::string, Raw> || std::is_same_v<std::string_view, Raw>) {
				result += part;
			} else {
				result += ToString(part);
			}
			result += delimeter;
		}
		if(result.size() > 0) {
			result.resize(result.size() - delimeter.size());
		}
		return result;
#endif
	}

    template<typename T>
    constexpr std::string Join(const std::string& delimeter, std::initializer_list<T> parts) {
        return Join(delimeter, std::ranges::subrange(parts.begin(), parts.end()));
    }

    constexpr std::string Join(char delimeter, auto parts) {
#if defined(__cpp_lib_ranges_join_with)
        return parts 
            | std::views::transform([](const auto& part) {
                using Raw = std::decay_t<decltype(part)>;
                if constexpr(std::is_same_v<Raw, std::string> || std::is_same_v<std::string_view, Raw>) {
                    return part;
                } else {
                    return ToString(part);
                }
            }) 
            | std::views::join_with(delimeter) 
            | std::ranges::to<std::string>();
#else
        std::string result;
        for(auto it = std::begin(parts); it != std::end(parts); it++) {
            auto part = *it;
            result += part;
            result += delimeter;
        }
        if(result.size() > 0) {
            result.resize(result.size() - 1);
        }
        return result;
#endif
    }

    template<typename T>
    constexpr std::string Join(char delimeter, std::initializer_list<T> parts) {
        return Join(delimeter, std::ranges::subrange(parts.begin(), parts.end()));
    }

    constexpr std::string HumanReadable(std::integral auto value) {
        using namespace std::string_view_literals;
        std::array suffixes {""sv, "K"sv, "M"sv, "B"sv, "T"sv, "Qa"sv, "Qi"sv, "Sx"sv, "Sp"sv, "Oc"sv, "No"sv, "Dc"sv};
        double scaled = static_cast<double>(value);
        size_t suffixIndex = 0;
        while(scaled >= 1000) {
            scaled /= 1000.0;
            ++suffixIndex;
        }
        suffixIndex = std::min(suffixIndex, suffixes.size() - 1);

        auto scaledInt = static_cast<int>(scaled);
        auto result = ToString(scaledInt);

        if(auto fraction = static_cast<int>((scaled - scaledInt) * 100); fraction > 0) {
            result += "." + ToString(fraction);
        }
        result += suffixes.at(suffixIndex);
        return result;
    }

    constexpr bool IsDigit(char c) { return c >= '0' && c <= '9'; }
    constexpr bool IsDigit(std::integral auto v) = delete;

    constexpr std::string TrimStart(std::string_view s, const std::vector<char>& toRemove = {' ', '\n', '\t', '\r'}) {
        return s | 
            std::views::drop_while([&](char c) { return std::find(toRemove.begin(), toRemove.end(), c) != toRemove.end(); })|
            std::ranges::to<std::string>();
    }

    constexpr std::string TrimEnd(std::string_view s, const std::vector<char>& toRemove = {' ', '\n', '\t', '\r'}) {
        return s | 
            std::views::reverse | 
            std::views::drop_while([&](char c) {return std::find(toRemove.begin(), toRemove.end(), c) != toRemove.end();}) |
			std::views::reverse |
			std::ranges::to<std::string>();
    }

    constexpr std::string Trim(std::string_view s, const std::vector<char>& toRemove = {' ', '\n', '\t', '\r'}) { 
        return TrimStart(TrimEnd(s, toRemove), toRemove);
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr std::string RemoveAll(std::string from, std::string_view what) {
        size_t pos = 0;
        while((pos = from.find(what, pos)) != std::string::npos) {
            from.erase(pos, what.length());
        }
        return from;
    }

    constexpr std::string TimeString(auto ms) {
        static_assert(std::is_integral_v<decltype(ms)>, "TimeString only supports integral types");
        auto seconds = ms / 1'000;
        auto minutes = seconds / 60;
        auto hours = minutes / 60;
        auto days = hours / 24;

        std::string result;
        if(days > 0) {
            result += Constexpr::ToString(days) + "d ";
        }
        if(hours % 24 > 0) {
            result += Constexpr::ToString(hours % 24) + "h ";
        }
        if(minutes % 60 > 0) {
            result += Constexpr::ToString(minutes % 60) + "m ";
        }
        if(seconds % 60 > 0) {
            result += Constexpr::ToString(seconds % 60) + "s";
        }
        if (result.empty()) {
            result = "0s";
        }

        return Constexpr::Trim(result);
    }

    constexpr char ToLower(char c) { 
        return c >= 'A' && c <= 'Z' ? static_cast<char>(c + ('a' - 'A')) : c;
    }
    constexpr char ToUpper(char c) { 
        return c >= 'a' && c <= 'z' ? static_cast<char>(c - ('a' - 'A')) : c; 
    }

    constexpr std::string ToLower(std::string_view input) {
        return input 
            | std::views::transform([](char c) { return Constexpr::ToLower(c); }) 
            | std::ranges::to<std::string>();
    }

    constexpr std::string ToUpper(std::string_view input) {
        return input 
            | std::views::transform([](char c) { return Constexpr::ToUpper(c); }) 
            | std::ranges::to<std::string>();
    }
}