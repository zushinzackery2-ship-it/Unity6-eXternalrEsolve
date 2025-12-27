#pragma once

#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

#include "gom_offsets.hpp"
#include "gom_pool.hpp"
#include "gom_search_common.hpp"
#include "../object/managed/managed_backend.hpp"
#include "../object/managed/managed_object.hpp"
#include "../object/native/native_component.hpp"
#include "../object/native/native_game_object.hpp"
#include "../core/offsets.hpp"

namespace er6
{

inline std::vector<std::uintptr_t> FindComponentsOnGameObjectThroughClassNameContainsAll(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    const GomOffsets& gomOff,
    std::uintptr_t gameObjectNative,
    std::initializer_list<std::string_view> parts)
{
    std::vector<std::uintptr_t> out;

    if (!Supports(backend, Feature::ManagedClassName))
    {
        return out;
    }

    if (!gameObjectNative || parts.size() == 0)
    {
        return out;
    }

    std::int32_t count = 0;
    if (!GetComponentCount(mem, gameObjectNative, gomOff, count) || count <= 0)
    {
        return out;
    }

    if (count > 1024)
    {
        count = 1024;
    }

    out.reserve(static_cast<std::size_t>(count));

    for (std::int32_t i = 0; i < count; ++i)
    {
        std::uintptr_t nativeComp = 0;
        if (!GetNativeGameObjectComponent(mem, gameObjectNative, gomOff, i, nativeComp) || !nativeComp)
        {
            continue;
        }

        std::uintptr_t managedComp = 0;
        if (!GetNativeComponentManaged(mem, nativeComp, gomOff, managedComp) || !managedComp)
        {
            continue;
        }

        std::string className;
        if (!ReadManagedObjectClassName(mem, managedComp, managedOff, className))
        {
            continue;
        }

        if (className.empty())
        {
            continue;
        }

        if (ContainsAllSubstrings(className, parts))
        {
            out.push_back(nativeComp);
        }
    }

    return out;
}

template <typename... Ts>
inline std::vector<std::uintptr_t> FindComponentsOnGameObjectThroughClassNameContainsAll(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    const GomOffsets& gomOff,
    std::uintptr_t gameObjectNative,
    Ts&&... parts)
{
    return FindComponentsOnGameObjectThroughClassNameContainsAll(
        backend,
        mem,
        managedOff,
        gomOff,
        gameObjectNative,
        std::initializer_list<std::string_view>{std::string_view(parts)...});
}

inline std::vector<std::uintptr_t> FindComponentsThroughClassNameContainsAll(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& gomOff,
    std::initializer_list<std::string_view> parts)
{
    std::vector<std::uintptr_t> out;

    if (!Supports(backend, Feature::ManagedClassName))
    {
        return out;
    }

    if (parts.size() == 0)
    {
        return out;
    }

    std::vector<ComponentEntry> comps;
    if (!EnumerateComponents(mem, gomGlobalSlot, gomOff, comps))
    {
        return out;
    }

    out.reserve(comps.size());

    for (const auto& c : comps)
    {
        if (!c.nativeComponent || !c.managedComponent)
        {
            continue;
        }

        std::string className;
        if (!ReadManagedObjectClassName(mem, c.managedComponent, managedOff, className))
        {
            continue;
        }

        if (className.empty())
        {
            continue;
        }

        if (ContainsAllSubstrings(className, parts))
        {
            out.push_back(c.nativeComponent);
        }
    }

    return out;
}

template <typename... Ts>
inline std::vector<std::uintptr_t> FindComponentsThroughClassNameContainsAll(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    std::uintptr_t gomGlobalSlot,
    const GomOffsets& gomOff,
    Ts&&... parts)
{
    return FindComponentsThroughClassNameContainsAll(
        backend,
        mem,
        managedOff,
        gomGlobalSlot,
        gomOff,
        std::initializer_list<std::string_view>{std::string_view(parts)...});
}

inline std::uintptr_t GetComponentThroughTypeId(
    const IMemoryAccessor& mem,
    const GomOffsets& off,
    std::uintptr_t gameObjectNative,
    std::int32_t typeId)
{
    if (!gameObjectNative)
    {
        return 0;
    }

    std::uintptr_t pool = 0;
    if (!GetComponentPool(mem, gameObjectNative, off, pool))
    {
        return 0;
    }

    std::int32_t count = 0;
    if (!GetComponentCount(mem, gameObjectNative, off, count))
    {
        return 0;
    }

    if (count <= 0 || count > 1024)
    {
        return 0;
    }

    for (int i = 0; i < count; ++i)
    {
        std::int32_t typeIdValue = 0;
        if (!GetComponentSlotTypeId(mem, pool, off, i, typeIdValue))
        {
            continue;
        }

        if (typeIdValue != typeId)
        {
            continue;
        }

        std::uintptr_t nativeComp = 0;
        if (GetComponentSlotNative(mem, pool, off, i, nativeComp))
        {
            return nativeComp;
        }
    }

    return 0;
}

inline std::uintptr_t GetComponentThroughTypeName(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    const GomOffsets& gomOff,
    std::uintptr_t gameObjectNative,
    const std::string& typeName)
{
    if (!Supports(backend, Feature::ManagedClassName))
    {
        return 0;
    }

    if (!gameObjectNative)
    {
        return 0;
    }

    std::uintptr_t pool = 0;
    if (!GetComponentPool(mem, gameObjectNative, gomOff, pool))
    {
        return 0;
    }

    std::int32_t count = 0;
    if (!GetComponentCount(mem, gameObjectNative, gomOff, count))
    {
        return 0;
    }

    if (count <= 0 || count > 1024)
    {
        return 0;
    }

    for (int i = 0; i < count; ++i)
    {
        std::uintptr_t nativeComp = 0;
        if (!GetComponentSlotNative(mem, pool, gomOff, i, nativeComp))
        {
            continue;
        }

        std::uintptr_t managedComp = 0;
        if (!GetNativeComponentManaged(mem, nativeComp, gomOff, managedComp) || !managedComp)
        {
            continue;
        }

        TypeInfo info;
        if (ReadManagedObjectTypeInfo(mem, managedComp, managedOff, info) && info.name == typeName)
        {
            return nativeComp;
        }
    }

    return 0;
}

inline std::uintptr_t FindGameObjectWithComponent(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    const GomOffsets& gomOff,
    std::uintptr_t gomGlobalSlot,
    const std::string& typeName)
{
    std::vector<GameObjectEntry> gameObjects;
    if (!EnumerateGameObjects(mem, gomGlobalSlot, gomOff, gameObjects))
    {
        return 0;
    }

    for (const auto& go : gameObjects)
    {
        if (!go.nativeObject)
        {
            continue;
        }

        if (GetComponentThroughTypeName(backend, mem, managedOff, gomOff, go.nativeObject, typeName))
        {
            return go.nativeObject;
        }
    }

    return 0;
}

} // namespace er6
