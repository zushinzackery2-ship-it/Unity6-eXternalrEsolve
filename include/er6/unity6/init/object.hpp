#pragma once

#include <optional>

#include "context.hpp"

#include "../object/native/native_game_object_name.hpp"
#include "../object/native/native_scriptable_object_name.hpp"
#include "../object/native/native_game_object.hpp"
#include "../object/managed/managed_object.hpp"

namespace er6
{

inline bool ReadGameObjectName(std::uintptr_t gameObject, std::string& outName)
{
    if (!IsInited())
    {
        return false;
    }

    return er6::ReadGameObjectName(Mem(), gameObject, g_ctx.off, outName);
}

inline std::optional<std::string> ReadGameObjectName(std::uintptr_t gameObject)
{
    std::string name;
    if (!ReadGameObjectName(gameObject, name))
    {
        return std::nullopt;
    }
    return name;
}

inline bool ReadScriptableObjectName(std::uintptr_t scriptableObject, std::string& outName)
{
    if (!IsInited())
    {
        return false;
    }

    return er6::ReadScriptableObjectName(Mem(), scriptableObject, g_ctx.off, outName);
}

inline std::optional<std::string> ReadScriptableObjectName(std::uintptr_t scriptableObject)
{
    std::string name;
    if (!ReadScriptableObjectName(scriptableObject, name))
    {
        return std::nullopt;
    }
    return name;
}

inline bool ReadNativeGameObjectName(std::uintptr_t nativeGo, std::string& outName)
{
    if (!IsInited())
    {
        return false;
    }

    return er6::ReadNativeGameObjectName(Mem(), nativeGo, g_ctx.gomOff, outName);
}

inline std::optional<std::string> ReadNativeGameObjectName(std::uintptr_t nativeGo)
{
    std::string name;
    if (!ReadNativeGameObjectName(nativeGo, name))
    {
        return std::nullopt;
    }
    return name;
}

inline bool ReadManagedObjectTypeInfo(std::uintptr_t managed, TypeInfo& out)
{
    if (!IsInited())
    {
        return false;
    }

    return er6::ReadManagedObjectTypeInfo(Mem(), managed, g_ctx.off, out);
}

inline std::optional<TypeInfo> ReadManagedObjectTypeInfo(std::uintptr_t managed)
{
    TypeInfo info;
    if (!ReadManagedObjectTypeInfo(managed, info))
    {
        return std::nullopt;
    }
    return info;
}

} // namespace er6
