#pragma once

#include <cstdint>

#include "../../mem/memory_read.hpp"
#include "gom_offsets.hpp"

namespace er6
{

inline bool GetListNodeFirst(const IMemoryAccessor& mem, std::uintptr_t listHead, const GomOffsets& off, std::uintptr_t& outNode)
{
    (void)mem;
    (void)off;
    outNode = 0;
    if (!listHead)
    {
        return false;
    }

    outNode = listHead;
    return true;
}

inline bool GetListNodeNext(const IMemoryAccessor& mem, std::uintptr_t node, const GomOffsets& off, std::uintptr_t& outNext)
{
    outNext = 0;
    if (!node)
    {
        return false;
    }

    return ReadPtr(mem, node + off.node.next, outNext);
}

inline bool GetListNodeNative(const IMemoryAccessor& mem, std::uintptr_t node, const GomOffsets& off, std::uintptr_t& outNative)
{
    outNative = 0;
    if (!node)
    {
        return false;
    }

    return ReadPtr(mem, node + off.node.native_object, outNative);
}

} // namespace er6
