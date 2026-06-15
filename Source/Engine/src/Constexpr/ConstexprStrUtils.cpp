#include "Constexpr/ConstexprStrUtils.h"

#include <array>
#include <vector>

namespace Constexpr {
    using namespace std::literals;

    static_assert(ToString(123) == "123");
    static_assert(ToString(0) == "0");

    static_assert(TryFromString<int>("123") == 123);
    static_assert(TryFromString<int>("-123") == -123);
    static_assert(TryFromString<int>("abc") == std::nullopt);

    static_assert(Split("Hello World", " ") == std::vector<std::string_view>{"Hello"sv, "World"sv});
    static_assert(Split("Hello   World", " ") == std::vector<std::string_view>{"Hello"sv, "World"sv});
    static_assert(Split("Hello   World", " ", true) == std::vector<std::string_view>{"Hello"sv, ""sv, ""sv, "World"sv});
    static_assert(Split("Hello", " ") == std::vector<std::string_view>{"Hello"sv});
    static_assert(Split("", " ").empty());
    static_assert(Split(" ", " ").empty());

    static_assert(Join(", ", std::vector{1, 2, 3}) == "1, 2, 3");
    static_assert(Join(", ", {1, 2, 3}) == "1, 2, 3");
    static_assert(Join(' ', std::vector{1, 2, 3}) == "1 2 3");
    static_assert(Join(' ', {1, 2, 3}) == "1 2 3");
    static_assert(Join(" ", {"A", "B", "C"}) == "A B C");
    static_assert(Join(" ", {"A"sv, "B"sv, "C"sv}) == "A B C");
    static_assert(Join(" ", {"A"s, "B"s, "C"s}) == "A B C");

    static_assert(HumanReadable(0) == "0");
    static_assert(HumanReadable(999) == "999");
    static_assert(HumanReadable(1'000) == "1K");
    static_assert(HumanReadable(1'234) == "1.23K");
    static_assert(HumanReadable(1'500) == "1.50K");
    static_assert(HumanReadable(2'500'000) == "2.50M");
    static_assert(HumanReadable(3'600'000'000) == "3.60B");

    static_assert(IsDigit('0'));
    static_assert(IsDigit('9'));
    static_assert(!IsDigit('a'));
    static_assert(!IsDigit('Z'));

    static_assert(TrimStart("Hello") == "Hello");
    static_assert(TrimStart("  Hello") == "Hello");
    static_assert(TrimStart("Hello  ") == "Hello  ");
    static_assert(TrimEnd("Hello") == "Hello");
    static_assert(TrimEnd("Hello  ") == "Hello");
    static_assert(Trim("  Hello  ") == "Hello");
    static_assert(Trim("\t\r\n Hello \t\r\n") == "Hello");

    constexpr auto OneSecond = 1'000;
    constexpr auto OneMinute = OneSecond * 60;
    constexpr auto OneHour = OneMinute * 60;
    constexpr auto OneDay = OneHour * 24;

    static_assert(TimeString(OneSecond) == "1s");
    static_assert(TimeString(OneMinute) == "1m");
    static_assert(TimeString(OneHour) == "1h");
    static_assert(TimeString(OneDay) == "1d");
    static_assert(TimeString(OneDay + OneHour + OneMinute + OneSecond) == "1d 1h 1m 1s");
}