#pragma once

#include "Core/Instrumentation/ISink.h"

struct GameLog : public Log::ISink {
	GameLog(Log::Filter filter);

	void Write(const Log::Entry& entry) override;
};