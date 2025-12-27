#pragma once

#include <optional>

#include "context.hpp"

#include "../transform/transform.hpp"

namespace er6
{

inline bool GetTransformWorldPosition(std::uintptr_t transformAddress, glm::vec3& outPos, int maxDepth)
{
    if (!IsInited())
    {
        return false;
    }

    return er6::GetTransformWorldPosition(Mem(), g_ctx.transformOff, transformAddress, outPos, maxDepth);
}

inline std::optional<glm::vec3> GetTransformWorldPosition(std::uintptr_t transformAddress, int maxDepth = 64)
{
    glm::vec3 pos(0.0f);
    if (!GetTransformWorldPosition(transformAddress, pos, maxDepth))
    {
        return std::nullopt;
    }
    return pos;
}

} // namespace er6
