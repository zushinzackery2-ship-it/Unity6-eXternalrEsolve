#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "../../os/win/win_module.hpp"
#include "../core/offsets.hpp"
#include "../metadata/pe.hpp"
#include "../object/native/native_object.hpp"
#include "ms_id_to_pointer.hpp"
#include "../../mem/memory_read.hpp"

namespace er6
{

inline bool IsReadableByte(const IMemoryAccessor& mem, std::uintptr_t address)
{
    if (!address)
    {
        return false;
    }

    std::uint8_t b = 0;
    return mem.Read(address, &b, 1);
}

inline std::uint32_t CountUnityObjectsInMsIdEntriesPool(
    const IMemoryAccessor& mem,
    std::uintptr_t entriesBase,
    std::uint32_t capacity,
    const Offsets& off,
    const UnityPlayerRange& unityPlayer)
{
    if (!IsCanonicalUserPtr(entriesBase))
    {
        return 0;
    }
    if (capacity == 0 || capacity > 50000000)
    {
        return 0;
    }

    const std::uintptr_t moduleBase = unityPlayer.base;
    const std::uintptr_t moduleEnd = unityPlayer.base + static_cast<std::uintptr_t>(unityPlayer.size);

    const std::uint32_t kChunkEntries = 65536;
    std::vector<std::uint8_t> chunk;
    chunk.resize(static_cast<std::size_t>(kChunkEntries) * static_cast<std::size_t>(off.ms_id_entry_stride));

    const std::uint32_t kMaxSamples = 50000;
    std::uint32_t step = 1;
    std::uint32_t maxIters = capacity;
    if (capacity > kMaxSamples)
    {
        step = capacity / kMaxSamples;
        if (step == 0)
        {
            step = 1;
        }
        maxIters = kMaxSamples;
    }

    std::uint32_t objCount = 0;
    std::uint32_t iters = 0;
    for (std::uint32_t base = 0; base < capacity && iters < maxIters; base += kChunkEntries)
    {
        std::uint32_t current = capacity - base;
        if (current > kChunkEntries)
        {
            current = kChunkEntries;
        }

        const std::uintptr_t chunkVa = entriesBase + static_cast<std::uintptr_t>(base) * off.ms_id_entry_stride;
        const std::size_t chunkBytes = static_cast<std::size_t>(current) * static_cast<std::size_t>(off.ms_id_entry_stride);
        if (!mem.Read(chunkVa, chunk.data(), chunkBytes))
        {
            continue;
        }

        for (std::uint32_t j = 0; j < current && iters < maxIters; ++j)
        {
            const std::uint32_t slotIndex = base + j;
            if (slotIndex % step != 0)
            {
                continue;
            }
            ++iters;

            const std::size_t entryOff = static_cast<std::size_t>(j) * static_cast<std::size_t>(off.ms_id_entry_stride);
            const MsIdToPointerEntryRaw* entry = reinterpret_cast<const MsIdToPointerEntryRaw*>(chunk.data() + entryOff);

            const std::uint32_t key = entry->key;
            if (key == 0 || key == 0xFFFFFFFFu || key == 0xFFFFFFFEu)
            {
                continue;
            }

            const std::uintptr_t obj = entry->object;
            if (!IsCanonicalUserPtr(obj))
            {
                continue;
            }

            std::uintptr_t vtable = 0;
            if (!ReadPtr(mem, obj, vtable))
            {
                continue;
            }
            if (vtable < moduleBase || vtable >= moduleEnd)
            {
                continue;
            }

            std::uintptr_t gchandle = 0;
            if (!ReadPtr(mem, obj + off.unity_object_gchandle_ptr, gchandle))
            {
                continue;
            }
            if (!IsCanonicalUserPtr(gchandle))
            {
                continue;
            }

            std::uintptr_t managed = 0;
            if (!ReadPtr(mem, gchandle + off.gchandle_to_managed, managed))
            {
                continue;
            }
            if (!IsCanonicalUserPtr(managed))
            {
                continue;
            }

            std::uintptr_t klass = 0;
            if (!ReadPtr(mem, managed + off.managed_to_klass, klass))
            {
                continue;
            }
            if (!IsCanonicalUserPtr(klass))
            {
                continue;
            }

            ++objCount;
        }
    }

    return objCount;
}

inline bool FindMsIdToPointerSlotVaByScan(
    const IMemoryAccessor& mem,
    const ModuleInfo& unityPlayer,
    const Offsets& off,
    const UnityPlayerRange& unityPlayerRange,
    std::uintptr_t& outMsIdToPointerSlotVa,
    std::uint32_t* outBestObjCount = nullptr)
{
    outMsIdToPointerSlotVa = 0;
    if (outBestObjCount)
    {
        *outBestObjCount = 0;
    }

    std::uint32_t sizeOfImage = 0;
    std::vector<ModuleSection> sections;
    if (!ReadModuleSections(mem, unityPlayer.base, sizeOfImage, sections))
    {
        return false;
    }

    std::uintptr_t bestAddr = 0;
    std::uint32_t bestObjCount = 0;

    for (const auto& s : sections)
    {
        if (!(s.name == ".data" || s.name == ".rdata"))
        {
            continue;
        }
        if (s.size == 0)
        {
            continue;
        }

        std::vector<std::uint8_t> buf;
        buf.resize(s.size);

        const std::uintptr_t secVa = unityPlayer.base + static_cast<std::uintptr_t>(s.rva);
        if (!mem.Read(secVa, buf.data(), buf.size()))
        {
            continue;
        }

        for (std::size_t i = 0; i + sizeof(std::uintptr_t) <= buf.size(); i += sizeof(std::uintptr_t))
        {
            std::uintptr_t ptr = 0;
            std::memcpy(&ptr, buf.data() + i, sizeof(ptr));
            if (!IsCanonicalUserPtr(ptr))
            {
                continue;
            }

            std::uint8_t baseData[0x10] = {};
            if (!mem.Read(ptr, baseData, sizeof(baseData)))
            {
                continue;
            }

            std::uintptr_t entriesBase = 0;
            std::memcpy(&entriesBase, baseData + static_cast<std::size_t>(off.ms_id_set_entries_base), sizeof(entriesBase));
            if (!IsCanonicalUserPtr(entriesBase))
            {
                continue;
            }
            if (!IsReadableByte(mem, entriesBase))
            {
                continue;
            }

            std::uint32_t capacity = 0;
            std::memcpy(&capacity, baseData + static_cast<std::size_t>(off.ms_id_set_capacity), sizeof(capacity));
            if (capacity == 0 || capacity > 50000000)
            {
                continue;
            }

            std::uint32_t count = 0;
            std::memcpy(&count, baseData + static_cast<std::size_t>(off.ms_id_set_count), sizeof(count));
            if (count == 0 || count > 5000000 || count > capacity)
            {
                continue;
            }

            const std::uint32_t objCount = CountUnityObjectsInMsIdEntriesPool(mem, entriesBase, capacity, off, unityPlayerRange);
            if (objCount == 0)
            {
                continue;
            }

            const std::uintptr_t addr = secVa + static_cast<std::uintptr_t>(i);
            if (objCount > bestObjCount)
            {
                bestObjCount = objCount;
                bestAddr = addr;
            }
        }
    }

    if (!bestAddr)
    {
        return false;
    }

    outMsIdToPointerSlotVa = bestAddr;
    if (outBestObjCount)
    {
        *outBestObjCount = bestObjCount;
    }
    return true;
}

inline bool FindMsIdToPointerSlotRvaByScan(
    const IMemoryAccessor& mem,
    std::uintptr_t unityPlayerBase,
    const Offsets& off,
    std::uint64_t& outMsIdToPointerSlotRva,
    std::uint32_t* outBestObjCount = nullptr)
{
    outMsIdToPointerSlotRva = 0;
    if (!unityPlayerBase)
    {
        return false;
    }

    std::uint32_t sizeOfImage = 0;
    std::vector<ModuleSection> sections;
    if (!ReadModuleSections(mem, unityPlayerBase, sizeOfImage, sections))
    {
        return false;
    }

    ModuleInfo unityPlayer;
    unityPlayer.base = unityPlayerBase;
    unityPlayer.size = sizeOfImage;

    UnityPlayerRange range;
    range.base = unityPlayer.base;
    range.size = unityPlayer.size;

    std::uintptr_t slotVa = 0;
    if (!FindMsIdToPointerSlotVaByScan(mem, unityPlayer, off, range, slotVa, outBestObjCount))
    {
        return false;
    }

    outMsIdToPointerSlotRva = static_cast<std::uint64_t>(slotVa - unityPlayer.base);
    return true;
}

} // namespace er6
