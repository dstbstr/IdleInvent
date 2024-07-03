#pragma once

#include "Core/NumTypes.h"

#include <vector>
#include <string>
#include <array>
#include <charconv>

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
}