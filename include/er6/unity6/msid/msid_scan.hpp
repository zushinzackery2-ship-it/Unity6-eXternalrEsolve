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
    std::uint32_t count,
    const Offsets& off,
    const UnityPlayerRange& unityPlayer)
{
    if (!IsCanonicalUserPtr(entriesBase))
    {
        return 0;
    }
    if (count == 0 || count > 5000000)
    {
        return 0;
    }

    const std::uint64_t poolSizeU64 = static_cast<std::uint64_t>(count) * static_cast<std::uint64_t>(off.ms_id_entry_stride);
    if (poolSizeU64 == 0 || poolSizeU64 > 128ull * 1024ull * 1024ull)
    {
        return 0;
    }
    const std::size_t poolSize = static_cast<std::size_t>(poolSizeU64);

    std::vector<std::uint8_t> pool;
    pool.resize(poolSize);
    if (!mem.Read(entriesBase, pool.data(), pool.size()))
    {
        return 0;
    }

    const std::uintptr_t moduleBase = unityPlayer.base;
    const std::uintptr_t moduleEnd = unityPlayer.base + static_cast<std::uintptr_t>(unityPlayer.size);

    std::uint8_t objBuf[0x20] = {};

    std::uint32_t objCount = 0;
    const std::uint32_t total = count;
    for (std::uint32_t i = 0; i < total; ++i)
    {
        const std::size_t entryOff = static_cast<std::size_t>(i) * static_cast<std::size_t>(off.ms_id_entry_stride);
        if (entryOff + static_cast<std::size_t>(off.ms_id_entry_object) + sizeof(std::uintptr_t) > pool.size())
        {
            break;
        }

        std::uint32_t key = 0;
        std::memcpy(&key, pool.data() + entryOff + static_cast<std::size_t>(off.ms_id_entry_key), sizeof(key));
        if (key == 0xFFFFFFFFu || key == 0xFFFFFFFEu)
        {
            continue;
        }

        std::uintptr_t obj = 0;
        std::memcpy(&obj, pool.data() + entryOff + static_cast<std::size_t>(off.ms_id_entry_object), sizeof(obj));
        if (!IsCanonicalUserPtr(obj))
        {
            continue;
        }

        if (!mem.Read(obj, objBuf, sizeof(objBuf)))
        {
            continue;
        }

        std::uintptr_t vtable = 0;
        std::memcpy(&vtable, objBuf, sizeof(vtable));
        if (vtable < moduleBase || vtable >= moduleEnd)
        {
            continue;
        }

        if (static_cast<std::size_t>(off.unity_object_managed_ptr) + sizeof(std::uintptr_t) > sizeof(objBuf))
        {
            continue;
        }

        std::uintptr_t managed = 0;
        std::memcpy(&managed, objBuf + static_cast<std::size_t>(off.unity_object_managed_ptr), sizeof(managed));
        if (!IsCanonicalUserPtr(managed))
        {
            continue;
        }
        if (!IsReadableByte(mem, managed))
        {
            continue;
        }

        ++objCount;
        if (i >= 10000 && objCount == 0)
        {
            return 0;
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

            std::uint32_t count = 0;
            std::memcpy(&count, baseData + static_cast<std::size_t>(off.ms_id_set_count), sizeof(count));
            if (count == 0 || count > 5000000)
            {
                continue;
            }

            const std::uint32_t objCount = CountUnityObjectsInMsIdEntriesPool(mem, entriesBase, count, off, unityPlayerRange);
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
