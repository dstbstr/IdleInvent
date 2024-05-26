#ifdef __ANDROID__
#include "Platform/GameLog.h"

#include <android/log.h>

#ifdef DEBUG
#ifdef __ANDROID__
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP);
#else
#define DEBUG_BREAK __debugbreak();
#endif
#else
#define DEBUG_BREAK
#endif

namespace {
    constexpr char InfoLineFormat[]{
        "%s:%zu " // File:Line
        "%s\n" // Msg
    };

    constexpr char ErrorLineFormat[]{
        "%s:%zu " // File:Line
        "(%s) - " // Function name
        "%s\n" // Msg
    };

    constexpr auto LogTag = "Minimalist";
} // namespace

GameLog::GameLog(Log::Filter filter) : ISink(filter) {}

void GameLog::Write(const Log::Entry& entry) {
    switch(entry.LogLevel) {
    case Log::Level::Info: __android_log_print(ANDROID_LOG_INFO, LogTag, InfoLineFormat, entry.Context.FileName.c_str(), entry.Context.LineNumber, entry.Message.c_str()); break;
    case Log::Level::Warning: __android_log_print(ANDROID_LOG_WARN, LogTag, ErrorLineFormat, entry.Context.FileName.c_str(), entry.Context.LineNumber, entry.Context.Function.c_str(), entry.Message.c_str()); break;
    case Log::Level::Error:
        DEBUG_BREAK
        __android_log_print(ANDROID_LOG_ERROR, LogTag, ErrorLineFormat, entry.Context.FileName.c_str(), entry.Context.LineNumber, entry.Context.Function.c_str(), entry.Message.c_str());
        abort();
    }
}
#endif