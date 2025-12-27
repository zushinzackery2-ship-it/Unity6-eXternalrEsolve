#pragma once

#include <cstdint>
#include <string>

#include "../../../mem/memory_read.hpp"
#include "../../core/offsets.hpp"

namespace er6
{

inline bool IsValidAsciiIdent(const std::string& s, bool allowDot)
{
    if (s.empty() || s.size() > 128)
    {
        return false;
    }

    const char c0 = s[0];
    if (!((c0 >= 'A' && c0 <= 'Z') || (c0 >= 'a' && c0 <= 'z') || c0 == '_'))
    {
        return false;
    }

    for (char c : s)
    {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (uc < 0x20 || uc > 0x7E)
        {
            return false;
        }

        const bool okAlphaNum = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        const bool okExtra = (c == '_' || c == '`' || c == '+');
        const bool okDot = allowDot && c == '.';
        if (!okAlphaNum && !okExtra && !okDot)
        {
            return false;
        }
    }

    return true;
}

inline bool ReadIl2CppClassName(const IMemoryAccessor& mem, std::uintptr_t klass, const Offsets& off, std::string& outNs, std::string& outName)
{
    outNs.clear();
    outName.clear();

    if (!IsCanonicalUserPtr(klass))
    {
        return false;
    }

    std::uintptr_t namePtr = 0;
    std::uintptr_t nsPtr = 0;
    if (!ReadPtr(mem, klass + off.il2cppclass_name_ptr, namePtr))
    {
        return false;
    }
    if (!ReadPtr(mem, klass + off.il2cppclass_namespace_ptr, nsPtr))
    {
        return false;
    }

    std::string name;
    std::string ns;
    if (!ReadCString(mem, namePtr, name, 128))
    {
        return false;
    }

    if (nsPtr)
    {
        ReadCString(mem, nsPtr, ns, 128);
    }

    if (!IsValidAsciiIdent(name, false))
    {
        return false;
    }
    if (!ns.empty() && !IsValidAsciiIdent(ns, true))
    {
        return false;
    }

    outName = name;
    outNs = ns;
    return true;
}

inline bool IsClassOrParent(const IMemoryAccessor& mem, std::uintptr_t klass, const Offsets& off, const char* targetNs, const char* targetName)
{
    std::uintptr_t cur = klass;
    for (int i = 0; i < 64; ++i)
    {
        if (!IsCanonicalUserPtr(cur))
        {
            return false;
        }

        std::string ns;
        std::string name;
        if (ReadIl2CppClassName(mem, cur, off, ns, name))
        {
            if (name == targetName && ns == targetNs)
            {
                return true;
            }
        }

        std::uintptr_t parent = 0;
        if (!ReadPtr(mem, cur + off.il2cppclass_parent, parent))
        {
            return false;
        }
        cur = parent;
        if (!cur)
        {
            return false;
        }
    }

    return false;
}

} // namespace er6
