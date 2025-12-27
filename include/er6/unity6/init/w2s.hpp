#pragma once

#include "context.hpp"

#include "../camera/world_to_screen.hpp"

namespace er6
{

inline WorldToScreenResult W2S(const glm::mat4& viewProj, const ScreenRect& screen, const glm::vec3& world)
{
    return ::er6::WorldToScreenPoint(viewProj, screen, world);
}

} // namespace er6
