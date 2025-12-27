#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "memory_accessor.hpp"

namespace er6
{

template <typename T>
inline bool ReadValue(const IMemoryAccessor& mem, std::uintptr_t address, T& out)
{
    out = T{};
    if (!address)
    {
        return false;
    }
    return mem.Read(address, &out, sizeof(T));
}

inline bool ReadPtr(const IMemoryAccessor& mem, std::uintptr_t address, std::uintptr_t& out)
{
    out = 0;
    return ReadValue(mem, address, out);
}

inline bool ReadCString(const IMemoryAccessor& mem, std::uintptr_t address, std::string& out, std::size_t maxLen = 256)
{
    out.clear();
    if (!address || maxLen == 0)
    {
        return false;
    }

    std::size_t readLen = maxLen;
    if (readLen > 4096)
    {
        readLen = 4096;
    }

    std::vector<char> buf;
    buf.resize(readLen + 1);

    if (!mem.Read(address, buf.data(), readLen))
    {
        return false;
    }

    buf[readLen] = '\0';
    std::size_t n = 0;
    for (; n < readLen; ++n)
    {
        if (buf[n] == '\0')
        {
            break;
        }
    }

    out.assign(buf.data(), n);
    return true;
}

inline bool IsCanonicalUserPtr(std::uintptr_t p)
{
    if (!p)
    {
        return false;
    }
    if (p < 0x10000)
    {
        return false;
    }
    if (p >= 0x0000800000000000ULL)
    {
        return false;
    }
    return true;
}

} // namespace er6
