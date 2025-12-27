#pragma once

#include <cstdint>
#include <cmath>

#include "glm/glm.hpp"

#include "../camera/camera.hpp"
#include "../transform/transform.hpp"

namespace er6
{

inline bool IsFiniteFloat(float v)
{
    return std::isfinite(v) != 0;
}

inline bool IsFiniteVec3(const glm::vec3& v)
{
    return IsFiniteFloat(v.x) && IsFiniteFloat(v.y) && IsFiniteFloat(v.z);
}

inline bool ValidateTransformOffsets(
    const IMemoryAccessor& mem,
    const TransformOffsets& off,
    std::uintptr_t transformAddress,
    int maxDepth)
{
    glm::vec3 pos(0.0f);
    if (!GetTransformWorldPosition(mem, off, transformAddress, pos, maxDepth))
    {
        return false;
    }

    return IsFiniteVec3(pos);
}

inline bool ValidateCameraOffsets(
    const IMemoryAccessor& mem,
    const CameraOffsets& off,
    std::uintptr_t nativeCamera)
{
    float m[16] = {};
    if (!GetCameraViewProjMatrix(mem, nativeCamera, off, m))
    {
        return false;
    }

    double sumAbs = 0.0;
    for (int i = 0; i < 16; ++i)
    {
        if (!IsFiniteFloat(m[i]))
        {
            return false;
        }
        sumAbs += std::fabs((double)m[i]);
    }

    return sumAbs > 0.0001;
}

} // namespace er6
