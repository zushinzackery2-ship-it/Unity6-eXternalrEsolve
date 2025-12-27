#pragma once

#include <cstdint>
#include <string>

#include "../../../mem/memory_read.hpp"
#include "../../core/offsets.hpp"

namespace er6
{

inline bool ReadScriptableObjectName(const IMemoryAccessor& mem, std::uintptr_t scriptableObject, const Offsets& off, std::string& out)
{
    out.clear();

    std::uintptr_t namePtr = 0;
    if (!ReadPtr(mem, scriptableObject + static_cast<std::uintptr_t>(off.scriptable_object_name_ptr), namePtr))
    {
        return false;
    }

    if (!IsCanonicalUserPtr(namePtr))
    {
        return false;
    }

    return ReadCString(mem, namePtr, out, 256);
}

} // namespace er6
