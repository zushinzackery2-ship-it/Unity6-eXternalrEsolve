#pragma once

#include <cstdint>

#include "../../mem/memory_read.hpp"
#include "../core/offsets.hpp"

namespace er6
{

#pragma pack(push, 1)
struct MsIdToPointerSetRaw
{
    std::uintptr_t entriesBase = 0;
    std::uint32_t capacity = 0;
    std::uint32_t count = 0;
};

struct MsIdToPointerEntryRaw
{
    std::uint32_t key = 0;
    std::uint32_t unk04 = 0;
    std::uint64_t unk08 = 0;
    std::uintptr_t object = 0;
};
#pragma pack(pop)

static_assert(sizeof(MsIdToPointerSetRaw) == 0x10, "MsIdToPointerSetRaw size");
static_assert(sizeof(MsIdToPointerEntryRaw) == 0x18, "MsIdToPointerEntryRaw size");

struct MsIdToPointerSet
{
    std::uintptr_t set = 0;
};

inline bool ReadMsIdToPointerSet(const IMemoryAccessor& mem, std::uintptr_t msIdToPointerAddr, MsIdToPointerSet& out)
{
    out = MsIdToPointerSet{};

    std::uintptr_t setPtr = 0;
    if (!ReadPtr(mem, msIdToPointerAddr, setPtr))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(setPtr))
    {
        return false;
    }

    out.set = setPtr;
    return true;
}

inline bool ReadMsIdEntriesHeader(
    const IMemoryAccessor& mem,
    const MsIdToPointerSet& set,
    const Offsets& off,
    std::uintptr_t& outEntriesBase,
    std::uint32_t& outCapacity,
    std::uint32_t& outCount)
{
    outEntriesBase = 0;
    outCapacity = 0;
    outCount = 0;

    if (!set.set)
    {
        return false;
    }

    std::uintptr_t entriesBase = 0;
    std::uint32_t capacity = 0;
    std::uint32_t count = 0;

    if (!ReadPtr(mem, set.set + off.ms_id_set_entries_base, entriesBase))
    {
        return false;
    }

    if (!ReadValue(mem, set.set + off.ms_id_set_capacity, capacity))
    {
        return false;
    }

    if (!ReadValue(mem, set.set + off.ms_id_set_count, count))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(entriesBase))
    {
        return false;
    }

    if (capacity == 0 || capacity > 50000000)
    {
        return false;
    }

    if (count == 0 || count > 5000000 || count > capacity)
    {
        return false;
    }

    outEntriesBase = entriesBase;
    outCapacity = capacity;
    outCount = count;
    return true;
}

} // namespace er6
