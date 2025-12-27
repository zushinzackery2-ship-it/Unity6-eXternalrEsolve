#pragma once

#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "gom_bucket.hpp"
#include "gom_list_node.hpp"
#include "gom_manager.hpp"
#include "gom_offsets.hpp"
#include "gom_search_common.hpp"
#include "../object/native/native_game_object.hpp"

namespace er6
{

inline std::uintptr_t FindGameObjectThroughTag(
    const IMemoryAccessor& mem,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& off,
    std::int32_t tag)
{
    std::uintptr_t managerAddress = 0;
    if (!GetGomManagerFromGlobalSlot(mem, gomGlobalSlot, managerAddress))
    {
        return 0;
    }

    const std::uintptr_t bucketPtr = FindBucketThroughTag(mem, managerAddress, off, tag);
    if (!bucketPtr)
    {
        return 0;
    }

    std::uintptr_t listHead = 0;
    if (!GetBucketListHead(mem, bucketPtr, off, listHead))
    {
        return 0;
    }

    std::uintptr_t node = 0;
    if (!GetListNodeFirst(mem, listHead, off, node))
    {
        return 0;
    }

    std::unordered_set<std::uintptr_t> visited;

    for (; node;)
    {
        if (visited.find(node) != visited.end())
        {
            break;
        }
        visited.insert(node);

        std::uintptr_t nativeObject = 0;
        if (!GetListNodeNative(mem, node, off, nativeObject))
        {
            break;
        }

        if (nativeObject)
        {
            std::int32_t tagValue = 0;
            if (GetNativeGameObjectTag(mem, nativeObject, off, tagValue) && tagValue == tag)
            {
                return nativeObject;
            }
        }

        std::uintptr_t next = 0;
        if (!GetListNodeNext(mem, node, off, next) || !next || next == listHead)
        {
            break;
        }

        node = next;
    }

    return 0;
}

inline std::uintptr_t FindGameObjectThroughName(
    const IMemoryAccessor& mem,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& off,
    const std::string& name)
{
    std::vector<GameObjectEntry> gameObjects;
    if (!EnumerateGameObjects(mem, gomGlobalSlot, off, gameObjects))
    {
        return 0;
    }

    for (const auto& go : gameObjects)
    {
        if (!go.nativeObject)
        {
            continue;
        }

        std::string goName;
        if (!ReadNativeGameObjectName(mem, go.nativeObject, off, goName))
        {
            continue;
        }

        if (goName == name)
        {
            return go.nativeObject;
        }
    }

    return 0;
}

inline std::vector<std::uintptr_t> FindGameObjectsThroughNameContainsAll(
    const IMemoryAccessor& mem,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& off,
    std::initializer_list<std::string_view> parts)
{
    std::vector<std::uintptr_t> out;
    if (parts.size() == 0)
    {
        return out;
    }

    std::vector<GameObjectEntry> gameObjects;
    if (!EnumerateGameObjects(mem, gomGlobalSlot, off, gameObjects))
    {
        return out;
    }

    out.reserve(gameObjects.size());
    for (const auto& go : gameObjects)
    {
        if (!go.nativeObject)
        {
            continue;
        }

        std::string goName;
        if (!ReadNativeGameObjectName(mem, go.nativeObject, off, goName))
        {
            continue;
        }

        if (goName.empty())
        {
            continue;
        }

        if (ContainsAllSubstrings(goName, parts))
        {
            out.push_back(go.nativeObject);
        }
    }

    return out;
}

template <typename... Ts>
inline std::vector<std::uintptr_t> FindGameObjectsThroughNameContainsAll(
    const IMemoryAccessor& mem,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& off,
    Ts&&... parts)
{
    return FindGameObjectsThroughNameContainsAll(mem, gomGlobalSlot, off, std::initializer_list<std::string_view>{std::string_view(parts)...});
}

} // namespace er6
