#ifdef __ANDROID__
#include "Platform/DebugStream.h"
#include <android/log.h>

struct DebugStream::DebugStreamBuf : public std::stringbuf {
    int sync() override {
        auto msg = str();
        __android_log_print(ANDROID_LOG_INFO, "IdleInvent", "%s", msg.c_str());
        str("");
        return 0;
    }
};

DebugStream::DebugStream() : DebugStream(std::make_unique<DebugStream::DebugStreamBuf>()) {}

DebugStream::DebugStream(std::unique_ptr<DebugStreamBuf>&& buf) : std::ostream(buf.get()), m_Buf(std::move(buf)) {}

DebugStream::~DebugStream() {
    m_Buf->sync();
    m_Buf.reset();
}
#endif