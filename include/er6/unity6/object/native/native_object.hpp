#pragma once

#include <cstdint>

#include "../../../mem/memory_read.hpp"
#include "../../core/offsets.hpp"

namespace er6
{

struct UnityPlayerRange
{
    std::uintptr_t base = 0;
    std::uint32_t size = 0;

    bool Contains(std::uintptr_t p) const
    {
        if (!base || size == 0)
        {
            return false;
        }
        return p >= base && p < base + static_cast<std::uintptr_t>(size);
    }
};

inline bool IsProbablyUnityObject(const IMemoryAccessor& mem, std::uintptr_t obj, const Offsets& off, const UnityPlayerRange& unityPlayer)
{
    if (!IsCanonicalUserPtr(obj))
    {
        return false;
    }

    if ((obj & 0x7) != 0)
    {
        return false;
    }

    std::uintptr_t vtable = 0;
    if (!ReadPtr(mem, obj, vtable))
    {
        return false;
    }

    if (!unityPlayer.Contains(vtable))
    {
        return false;
    }

    std::uintptr_t gchandle = 0;
    if (!ReadPtr(mem, obj + off.unity_object_gchandle_ptr, gchandle))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(gchandle))
    {
        return false;
    }

    std::uintptr_t managed = 0;
    if (!ReadPtr(mem, gchandle + off.gchandle_to_managed, managed))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(managed))
    {
        return false;
    }

    return true;
}

inline bool ReadUnityObjectInstanceId(const IMemoryAccessor& mem, std::uintptr_t obj, const Offsets& off, std::uint32_t& out)
{
    out = 0;
    return ReadValue(mem, obj + off.unity_object_instance_id, out);
}

inline bool ReadUnityObjectKlass(const IMemoryAccessor& mem, std::uintptr_t obj, const Offsets& off, std::uintptr_t& outKlass)
{
    outKlass = 0;

    std::uintptr_t gchandle = 0;
    if (!ReadPtr(mem, obj + off.unity_object_gchandle_ptr, gchandle))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(gchandle))
    {
        return false;
    }

    std::uintptr_t managed = 0;
    if (!ReadPtr(mem, gchandle + off.gchandle_to_managed, managed))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(managed))
    {
        return false;
    }

    std::uintptr_t klass = 0;
    if (!ReadPtr(mem, managed + off.managed_to_klass, klass))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(klass))
    {
        return false;
    }

    outKlass = klass;
    return true;
}

} // namespace er6
