#pragma once

#include <sstream>
#include <memory>

struct DebugStream : public std::ostream {
    DebugStream();
    ~DebugStream() noexcept override;
    DebugStream(const DebugStream&) = delete;
    DebugStream& operator=(const DebugStream&) = delete;
    DebugStream(DebugStream&&) = delete;
    DebugStream& operator=(DebugStream&&) = delete;

private:
    struct DebugStreamBuf;
    DebugStream(std::unique_ptr<DebugStreamBuf>&& buf);

    std::unique_ptr<DebugStreamBuf> m_Buf;
};