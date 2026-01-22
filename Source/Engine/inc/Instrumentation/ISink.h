#pragma once

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Instrumentation/Logging.h"

#include <optional>
#include <regex>
#include <functional>

template<typename T>
struct PubSub;

namespace Log {
	struct Filter {
		Filter& WithLevel(Level level);
		Filter& WithFilePattern(const std::regex& pattern);
		Filter& WithCustomMatcher(std::function<bool(const Entry&)> matcher);

		bool Matches(const Entry& entry) const;

	private:
		std::optional<Level> m_Level;
		std::optional<std::regex> m_FilePattern;
		std::optional<std::function<bool(const Entry&)>> m_CustomMatcher;
	};

	struct ISink {
		ISink(Filter filter);
		virtual ~ISink();

		virtual void Write(const Entry& entry) = 0;

	private:
		Handle m_Handle{ InvalidHandle };
		Filter m_Filter;
	};
}