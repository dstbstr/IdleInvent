#pragma once

#include <sstream>
#include <memory>

struct DebugStream : public std::ostream {
    DebugStream();
    ~DebugStream() noexcept;

private:
    struct DebugStreamBuf;
    DebugStream(std::unique_ptr<DebugStreamBuf>&& buf);

    std::unique_ptr<DebugStreamBuf> m_Buf;
};