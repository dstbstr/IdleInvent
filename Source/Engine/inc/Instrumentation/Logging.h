#pragma once

#include "Instrumentation/DebugContext.h"

#include <source_location>
#include <string>
#include <chrono>
#include <format>
#include <ostream>

namespace Log {
	enum struct Level {
		Debug,
		Info,
		Warning,
		Error
	};

	struct Entry {
		Entry(Level level, std::string msg, Debug::Context context)
			: LogLevel(level)
			, Message(msg)
			, Time(std::chrono::system_clock::now())
			, Context(context)
		{}

		Level LogLevel{};
		std::string Message{};
		std::chrono::system_clock::time_point Time{};
		Debug::Context Context{};
	};

	void Flush();

	void Debug(const std::string& message, std::source_location loc = std::source_location::current());
	void Info(const std::string& message, std::source_location loc = std::source_location::current());
	void Warn(const std::string& message, std::source_location loc = std::source_location::current());
	void Error(const std::string& message, std::source_location loc = std::source_location::current());

	#define DR_ASSERT_MSG(condition, message) \
		if(!(condition)) { \
			Log::Error(std::format("ASSERT: '{}' {}", #condition, message)); \
		}

	#define DR_ASSERT_MSG_LAZY(condition, lambda) \
		if(!(condition)) { \
			Log::Error(std::format("ASSERT: '{}' {}", #condition, lambda())); \
		}

	#define DR_ASSERT(condition) DR_ASSERT_MSG(condition, "")
}

constexpr std::string ToString(Log::Level level) {
	switch(level) {
    case Log::Level::Debug: return "DEBUG";
    case Log::Level::Info: return "INFO";
    case Log::Level::Warning: return "WARNING";
    case Log::Level::Error: return "ERROR";
    default: return "UNKNOWN";
	}
}
template<>
struct std::formatter<Log::Level> : std::formatter<std::string> {
    auto format(Log::Level level, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("{}", ToString(level)), ctx);    
	}
};

inline std::ostream& operator<<(std::ostream& os, const Log::Level& level) {
	//os << std::format("{}", level);
    os << ToString(level);
    return os;
}