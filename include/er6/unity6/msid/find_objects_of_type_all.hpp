#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../mem/memory_read.hpp"
#include "../core/offsets.hpp"
#include "../object/managed/il2cpp_class.hpp"
#include "../object/native/native_object.hpp"
#include "ms_id_to_pointer.hpp"

namespace er6
{

struct FindObjectsOfTypeAllResult
{
    std::uintptr_t object = 0;
    std::uint32_t instanceId = 0;
    std::uint32_t slot = 0;
};

inline bool FindObjectsOfTypeAll(
    const IMemoryAccessor& mem,
    std::uintptr_t msIdToPointerAddr,
    const Offsets& off,
    const UnityPlayerRange& unityPlayer,
    const char* className,
    std::vector<FindObjectsOfTypeAllResult>& out,
    const char* nameSpace = nullptr)
{
    out.clear();

    if (!className || !className[0]) return false;

    MsIdToPointerSet set;
    if (!ReadMsIdToPointerSet(mem, msIdToPointerAddr, set)) return false;

    std::uintptr_t entriesBase = 0;
    std::uint32_t capacity = 0;
    std::uint32_t count = 0;
    if (!ReadMsIdEntriesHeader(mem, set, off, entriesBase, capacity, count)) return false;

    const std::uint32_t kChunkSize = 65536;
    std::vector<std::uint8_t> buffer;
    buffer.resize(kChunkSize * off.ms_id_entry_stride);

    const std::uint32_t kObjHeaderSize = 0x60;
    std::uint8_t objBuffer[kObjHeaderSize];

    struct CachedClassInfo {
        bool valid;
        std::string ns;
        std::string cn;
    };
    std::unordered_map<std::uintptr_t, CachedClassInfo> classCache;

    for (std::uint32_t i = 0; i < capacity; i += kChunkSize)
    {
        std::uint32_t currentChunkCount = capacity - i;
        if (currentChunkCount > kChunkSize) currentChunkCount = kChunkSize;

        const std::uintptr_t chunkStart = entriesBase + static_cast<std::uintptr_t>(i) * off.ms_id_entry_stride;
        
        if (!mem.Read(chunkStart, buffer.data(), currentChunkCount * off.ms_id_entry_stride)) continue;

        for (std::uint32_t j = 0; j < currentChunkCount; ++j)
        {
            const MsIdToPointerEntryRaw* raw = reinterpret_cast<const MsIdToPointerEntryRaw*>(buffer.data() + j * off.ms_id_entry_stride);

            const std::uint32_t key = raw->key;
            if (key == 0 || key == 0xFFFFFFFFu || key >= 0xFFFFFFFEu) continue;

            const std::uintptr_t obj = raw->object;
            
            if (!mem.Read(obj, objBuffer, kObjHeaderSize)) continue;

            std::uintptr_t vtable = *reinterpret_cast<std::uintptr_t*>(objBuffer);
            if (!IsCanonicalUserPtr(vtable)) continue;
            
            if (vtable < unityPlayer.base || vtable >= (unityPlayer.base + unityPlayer.size)) continue;

            if (off.unity_object_managed_ptr >= kObjHeaderSize) continue;
            std::uintptr_t managed = *reinterpret_cast<std::uintptr_t*>(objBuffer + off.unity_object_managed_ptr);
            if (!IsCanonicalUserPtr(managed)) continue;

            std::uintptr_t klass = 0;
            if (!ReadPtr(mem, managed, klass)) continue;

            std::string ns;
            std::string cn;

            auto it = classCache.find(klass);
            if (it != classCache.end())
            {
                if (!it->second.valid) continue;
                ns = it->second.ns;
                cn = it->second.cn;
            }
            else
            {
                CachedClassInfo cacheEntry;
                cacheEntry.valid = false;

                if (ReadIl2CppClassName(mem, klass, off, cacheEntry.ns, cacheEntry.cn))
                    cacheEntry.valid = true;

                classCache[klass] = cacheEntry;

                if (!cacheEntry.valid) continue;
                ns = cacheEntry.ns;
                cn = cacheEntry.cn;
            }

            if (cn != className) continue;

            if (nameSpace && nameSpace[0])
            {
                if (ns != nameSpace) continue;
            }

            FindObjectsOfTypeAllResult r;
            r.object = obj;
            r.instanceId = key;
            r.slot = i + j;
            out.push_back(r);
        }
    }

    return true;
}

} // namespace er6
