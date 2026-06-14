#pragma once

#include <sstream>
#include <memory>

struct DebugStream : public std::ostream {
    DebugStream();
    ~DebugStream() noexcept override;
    DebugStream(const DebugStream&) = default;
    DebugStream& operator=(const DebugStream&) = default;
    DebugStream(DebugStream&&) = default;
    DebugStream& operator=(DebugStream&&) = default;

private:
    struct DebugStreamBuf;
    DebugStream(std::unique_ptr<DebugStreamBuf>&& buf);

    std::unique_ptr<DebugStreamBuf> m_Buf;
};