#pragma once

#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

#include "gom_bucket.hpp"
#include "gom_manager.hpp"
#include "gom_offsets.hpp"
#include "gom_walker.hpp"
#include "gom_walker_types.hpp"

namespace er6
{

inline bool ContainsAllSubstrings(const std::string& text, std::initializer_list<std::string_view> parts)
{
    if (parts.size() == 0)
    {
        return false;
    }

    for (std::string_view p : parts)
    {
        if (p.empty())
        {
            continue;
        }

        if (text.find(p.data(), 0, p.size()) == std::string::npos)
        {
            return false;
        }
    }

    return true;
}

inline bool EnumerateGameObjects(
    const IMemoryAccessor& mem,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& gomOff,
    std::vector<GameObjectEntry>& out)
{
    GomWalker w(mem, gomGlobalSlot, gomOff);
    return w.EnumerateGameObjects(out);
}

inline bool EnumerateComponents(
    const IMemoryAccessor& mem,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& gomOff,
    std::vector<ComponentEntry>& out)
{
    GomWalker w(mem, gomGlobalSlot, gomOff);
    return w.EnumerateComponents(out);
}

inline bool GetAllLinkedBuckets(
    const IMemoryAccessor& mem,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& off,
    std::vector<std::uintptr_t>& outBuckets)
{
    outBuckets.clear();

    std::uintptr_t managerAddress = 0;
    if (!GetGomManagerFromGlobalSlot(mem, gomGlobalSlot, managerAddress))
    {
        return false;
    }

    return GetAllLinkedBucketsFromManager(mem, managerAddress, off, outBuckets);
}

} // namespace er6
