#pragma once

#include <cstdint>
#include <string>
#include <vector>

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

    if (!className || !className[0])
    {
        return false;
    }

    MsIdToPointerSet set;
    if (!ReadMsIdToPointerSet(mem, msIdToPointerAddr, set))
    {
        return false;
    }

    std::uintptr_t entriesBase = 0;
    std::uint32_t capacity = 0;
    std::uint32_t count = 0;
    if (!ReadMsIdEntriesHeader(mem, set, off, entriesBase, capacity, count))
    {
        return false;
    }

    const std::uint32_t total = capacity;

    for (std::uint32_t i = 0; i < total; ++i)
    {
        const std::uintptr_t entry = entriesBase + static_cast<std::uintptr_t>(i) * off.ms_id_entry_stride;

        MsIdToPointerEntryRaw raw;
        if (!mem.Read(entry, &raw, sizeof(raw)))
        {
            continue;
        }

        const std::uint32_t key = raw.key;
        if (key == 0 || key == 0xFFFFFFFFu || key >= 0xFFFFFFFEu)
        {
            continue;
        }

        const std::uintptr_t obj = raw.object;
        if (!IsProbablyUnityObject(mem, obj, off, unityPlayer))
        {
            continue;
        }

        std::uintptr_t klass = 0;
        if (!ReadUnityObjectKlass(mem, obj, off, klass))
        {
            continue;
        }

        std::string ns;
        std::string cn;
        if (!ReadIl2CppClassName(mem, klass, off, ns, cn))
        {
            continue;
        }

        if (cn != className)
        {
            continue;
        }

        if (nameSpace && nameSpace[0])
        {
            if (ns != nameSpace)
            {
                continue;
            }
        }

        FindObjectsOfTypeAllResult r;
        r.object = obj;
        r.instanceId = key;
        r.slot = i;
        out.push_back(r);
    }

    return true;
}

} // namespace er6
