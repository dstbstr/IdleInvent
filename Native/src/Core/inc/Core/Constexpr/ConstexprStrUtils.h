#pragma once

#include "Core/NumTypes.h"

#include <vector>
#include <string>
#include <array>
#include <charconv>
#include <ranges>

namespace Constexpr {
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

    constexpr std::string Join(std::string delimeter, auto parts) {
		std::string result;
		for(auto& part: parts) {
            using Raw = std::decay_t<decltype(part)>;

            if constexpr(std::is_same_v<std::string, Raw> || std::is_same_v<std::string_view, Raw>) {
				result += part;
			} else {
				result += std::to_string(part);
			}
			result += delimeter;
		}
		if(result.size() > 0) {
			result.resize(result.size() - delimeter.size());
		}
		return result;
	}

    constexpr std::string Join(char delimeter, auto parts) {
        std::string result;
        for(auto& part: parts) {
            result += part;
            result += delimeter;
        }
        if(result.size() > 0) {
            result.resize(result.size() - 1);
        }
        return result;
    }

    constexpr std::string HumanReadable(auto value) {
        using namespace std::string_literals;
        static_assert(std::is_integral_v<decltype(value)>, "HumanReadable only supports integral types");
        std::array suffixes {""s, "K"s, "M"s, "B"s, "T"s, "Qa"s, "Qi"s, "Sx"s, "Sp"s, "Oc"s, "No"s, "Dc"s};
        double scaled = static_cast<double>(value);
        size_t suffixIndex = 0;
        while(scaled >= 1000) {
            scaled /= 1000.0;
            ++suffixIndex;
        }

        auto scaledInt = static_cast<int>(scaled);
        std::array<char, 13> wholePart{};
        std::to_chars(wholePart.data(), wholePart.data() + wholePart.size(), scaledInt);

        if(auto fraction = static_cast<int>((scaled - scaledInt) * 100); fraction > 0) {
            std::array<char, 3> decimalPart{};
            std::to_chars(decimalPart.data(), decimalPart.data() + decimalPart.size(), fraction);        
            return std::string(wholePart.data()) + "." + std::string(decimalPart.data()) + suffixes[suffixIndex];
        } else {
            return std::string(wholePart.data()) + suffixes[suffixIndex];
        }
    }

    constexpr bool IsDigit(char c) { return c >= '0' && c <= '9'; }

    constexpr std::string ToString(auto val) {
        bool negate = false;
        if constexpr(std::is_signed_v<decltype(val)>) {
            if(val < 0) {
                negate = true;
                val = -val;
            }
        }

        std::string result;
        while(val >= 10) {
            result.push_back(static_cast<char>('0' + (val % 10)));
            val /= 10;
        }
        result.push_back(static_cast<char>('0' + (val % 10)));
        if(negate) {
            result.push_back('-');
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

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

}