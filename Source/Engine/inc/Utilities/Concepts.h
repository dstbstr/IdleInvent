#pragma once

#include <concepts>
#include <string>

template<typename T>
concept HasToString = requires(T t) {
	{ ToString(t) } -> std::convertible_to<std::string>;
};
template<typename T>
concept ToStringSelf = requires(T t) {
	{ t.ToString() } -> std::convertible_to<std::string>;
};

template<typename T>
concept HasDescribe = requires(T t) {
    { Describe(t) } -> std::convertible_to<std::string>;
};

template<typename T>
concept DescribeSelf = requires(T t) {
	{ t.Describe() } -> std::convertible_to<std::string>;
};

template<typename T>
concept HasId = requires { typename T::IdType;} && 
	std::is_enum_v<typename T::IdType> && 
	requires(T t) { { t.Id } -> std::convertible_to<typename T::IdType>; };