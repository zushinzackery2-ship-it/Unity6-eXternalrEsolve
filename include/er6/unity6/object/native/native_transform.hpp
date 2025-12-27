#pragma once

#include <cstdint>
#include <string>

#include "glm/glm.hpp"

#include "../../gom/gom_offsets.hpp"
#include "../../gom/gom_search.hpp"
#include "../managed/managed_backend.hpp"
#include "native_component.hpp"
#include "../../core/offsets.hpp"
#include "../../transform/transform.hpp"

namespace er6
{

inline bool FindTransformOnGameObjectThroughTypeId(
    const IMemoryAccessor& mem,
    const GomOffsets& gomOff,
    std::uintptr_t gameObjectNative,
    std::int32_t transformTypeId,
    std::uintptr_t& outTransformNative)
{
    outTransformNative = 0;

    if (!gameObjectNative)
    {
        return false;
    }

    const std::uintptr_t nativeComp = GetComponentThroughTypeId(mem, gomOff, gameObjectNative, transformTypeId);
    if (!nativeComp)
    {
        return false;
    }

    outTransformNative = nativeComp;
    return true;
}

inline bool FindTransformOnGameObjectThroughTypeName(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    const GomOffsets& gomOff,
    std::uintptr_t gameObjectNative,
    const std::string& typeName,
    std::uintptr_t& outTransformNative)
{
    outTransformNative = 0;

    if (!gameObjectNative)
    {
        return false;
    }

    const std::uintptr_t nativeComp = GetComponentThroughTypeName(backend, mem, managedOff, gomOff, gameObjectNative, typeName);
    if (!nativeComp)
    {
        return false;
    }

    outTransformNative = nativeComp;
    return true;
}

inline bool FindTransformOnGameObject(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    const GomOffsets& gomOff,
    std::uintptr_t gameObjectNative,
    std::uintptr_t& outTransformNative)
{
    return FindTransformOnGameObjectThroughTypeName(backend, mem, managedOff, gomOff, gameObjectNative, std::string("Transform"), outTransformNative);
}

inline bool GetNativeTransformManaged(const IMemoryAccessor& mem, std::uintptr_t nativeTransform, const GomOffsets& gomOff, std::uintptr_t& outManaged)
{
    return GetNativeComponentManaged(mem, nativeTransform, gomOff, outManaged);
}

inline bool GetNativeTransformGameObject(const IMemoryAccessor& mem, std::uintptr_t nativeTransform, const GomOffsets& gomOff, std::uintptr_t& outGameObject)
{
    return GetNativeComponentGameObject(mem, nativeTransform, gomOff, outGameObject);
}

inline bool GetGameObjectWorldPositionThroughTransformTypeId(
    const IMemoryAccessor& mem,
    const GomOffsets& gomOff,
    const TransformOffsets& transformOff,
    std::uintptr_t gameObjectNative,
    std::int32_t transformTypeId,
    glm::vec3& outPos,
    int maxDepth)
{
    std::uintptr_t nativeTransform = 0;
    if (!FindTransformOnGameObjectThroughTypeId(mem, gomOff, gameObjectNative, transformTypeId, nativeTransform))
    {
        return false;
    }

    return GetTransformWorldPosition(mem, transformOff, nativeTransform, outPos, maxDepth);
}

inline bool GetGameObjectWorldPositionThroughTransformTypeName(
    ManagedBackend backend,
    const IMemoryAccessor& mem,
    const Offsets& managedOff,
    const GomOffsets& gomOff,
    const TransformOffsets& transformOff,
    std::uintptr_t gameObjectNative,
    const std::string& typeName,
    glm::vec3& outPos,
    int maxDepth)
{
    std::uintptr_t nativeTransform = 0;
    if (!FindTransformOnGameObjectThroughTypeName(backend, mem, managedOff, gomOff, gameObjectNative, typeName, nativeTransform))
    {
        return false;
    }

    return GetTransformWorldPosition(mem, transformOff, nativeTransform, outPos, maxDepth);
}

} // namespace er6
