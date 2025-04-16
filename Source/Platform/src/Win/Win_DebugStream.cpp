#ifdef WIN32
#include "Platform/DebugStream.h"

#include <iostream>

struct DebugStream::DebugStreamBuf : public std::stringbuf {
    int sync() override {
        std::cout << str();
        str("");
        return 0;
    }
};

DebugStream::DebugStream() : DebugStream(std::make_unique<DebugStream::DebugStreamBuf>()) {}

DebugStream::DebugStream(std::unique_ptr<DebugStreamBuf>&& buf) 
    : std::ostream(buf.get())
    , m_Buf(std::move(buf)) {}

DebugStream::~DebugStream() {
    m_Buf->sync();
    m_Buf.reset();
}
#endif