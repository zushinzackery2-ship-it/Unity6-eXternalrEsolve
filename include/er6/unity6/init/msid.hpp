#pragma once

#include <optional>

#include "context.hpp"

#include "../msid/ms_id_to_pointer.hpp"
#include "../msid/enumerate_objects.hpp"
#include "../msid/find_objects_of_type_all.hpp"

namespace er6
{

inline std::uintptr_t MsIdSetPtr()
{
    if (!IsInited() || !g_ctx.msIdToPointerSlotVa)
    {
        return 0;
    }

    MsIdToPointerSet set;
    if (!er6::ReadMsIdToPointerSet(Mem(), g_ctx.msIdToPointerSlotVa, set))
    {
        return 0;
    }
    return set.set;
}

inline std::uintptr_t MsIdEntriesBase()
{
    if (!IsInited() || !g_ctx.msIdToPointerSlotVa)
    {
        return 0;
    }

    MsIdToPointerSet set;
    if (!er6::ReadMsIdToPointerSet(Mem(), g_ctx.msIdToPointerSlotVa, set))
    {
        return 0;
    }

    std::uintptr_t entriesBase = 0;
    std::uint32_t capacity = 0;
    std::uint32_t count = 0;
    if (!er6::ReadMsIdEntriesHeader(Mem(), set, g_ctx.off, entriesBase, capacity, count))
    {
        return 0;
    }
    return entriesBase;
}

inline std::uint32_t MsIdCount()
{
    if (!IsInited() || !g_ctx.msIdToPointerSlotVa)
    {
        return 0;
    }

    MsIdToPointerSet set;
    if (!er6::ReadMsIdToPointerSet(Mem(), g_ctx.msIdToPointerSlotVa, set))
    {
        return 0;
    }

    std::uintptr_t entriesBase = 0;
    std::uint32_t capacity = 0;
    std::uint32_t count = 0;
    if (!er6::ReadMsIdEntriesHeader(Mem(), set, g_ctx.off, entriesBase, capacity, count))
    {
        return 0;
    }
    return count;
}

inline bool EnumerateMsIdToPointerObjects(const EnumerateOptions& opt, const std::function<void(const ObjectInfo&)>& cb)
{
    if (!IsInited() || !g_ctx.msIdToPointerSlotVa)
    {
        return false;
    }

    return er6::EnumerateMsIdToPointerObjects(Mem(), g_ctx.msIdToPointerSlotVa, g_ctx.off, g_ctx.unityPlayerRange, opt, cb);
}

inline std::optional<std::vector<ObjectInfo>> EnumerateMsIdToPointerObjects(const EnumerateOptions& opt)
{
    std::vector<ObjectInfo> out;
    const bool ok = EnumerateMsIdToPointerObjects(opt,
        [&](const ObjectInfo& info)
        {
            out.push_back(info);
        });

    if (!ok)
    {
        return std::nullopt;
    }

    return out;
}

inline bool FindObjectsOfTypeAll(const char* nameSpace, const char* className, std::vector<FindObjectsOfTypeAllResult>& out)
{
    if (!IsInited() || !g_ctx.msIdToPointerSlotVa)
    {
        out.clear();
        return false;
    }

    return er6::FindObjectsOfTypeAll(Mem(), g_ctx.msIdToPointerSlotVa, g_ctx.off, g_ctx.unityPlayerRange, className, out, nameSpace);
}

inline std::optional<std::vector<FindObjectsOfTypeAllResult>> FindObjectsOfTypeAll(const char* nameSpace, const char* className)
{
    std::vector<FindObjectsOfTypeAllResult> out;
    if (!FindObjectsOfTypeAll(nameSpace, className, out))
    {
        return std::nullopt;
    }
    return out;
}

inline std::optional<std::vector<FindObjectsOfTypeAllResult>> FindObjectsOfTypeAll(const char* className)
{
    return FindObjectsOfTypeAll(nullptr, className);
}

} // namespace er6
