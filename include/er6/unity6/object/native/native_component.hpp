#pragma once

#include <cstdint>

#include "../../../mem/memory_read.hpp"
#include "../../gom/gom_offsets.hpp"
#include "../../gom/gom_pool.hpp"

namespace er6
{

inline bool GetNativeComponentManaged(const IMemoryAccessor& mem, std::uintptr_t nativeComponent, const GomOffsets& off, std::uintptr_t& outManaged)
{
    outManaged = 0;
    if (!nativeComponent)
    {
        return false;
    }

    return ReadPtr(mem, nativeComponent + off.component.managed, outManaged) && outManaged != 0;
}

inline bool GetNativeComponentGameObject(const IMemoryAccessor& mem, std::uintptr_t nativeComponent, const GomOffsets& off, std::uintptr_t& outGameObject)
{
    outGameObject = 0;
    if (!nativeComponent)
    {
        return false;
    }

    return ReadPtr(mem, nativeComponent + off.component.game_object, outGameObject) && outGameObject != 0;
}

inline bool NativeComponent_GetGameObject(const IMemoryAccessor& mem, std::uintptr_t nativeComponent, const GomOffsets& off, std::uintptr_t& outGameObject)
{
    return GetNativeComponentGameObject(mem, nativeComponent, off, outGameObject);
}

inline bool IsComponentEnabled(const IMemoryAccessor& mem, std::uintptr_t nativeComponent, const GomOffsets& off)
{
    bool enabled = false;
    if (!GetComponentEnabled(mem, nativeComponent, off, enabled))
    {
        return false;
    }

    return enabled;
}

} // namespace er6
