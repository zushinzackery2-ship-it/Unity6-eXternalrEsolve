#pragma once

#include <cstdint>
#include <immintrin.h>

#include "glm/glm.hpp"

#include "../../mem/memory_read.hpp"

namespace er6
{

struct TransformOffsets
{
    std::uint32_t state_ptr = 0x28;
    std::uint32_t index = 0x30;

    std::uint32_t state_node_data = 0x18;
    std::uint32_t state_parent_indices = 0x20;

    std::uint32_t node_stride = 48;
};

struct TransformHierarchyState
{
    std::uintptr_t nodeData = 0;
    std::uintptr_t parentIndices = 0;
};

inline bool ReadTransformHierarchyState(
    const IMemoryAccessor& mem,
    std::uintptr_t transformAddress,
    const TransformOffsets& off,
    TransformHierarchyState& outState,
    std::int32_t& outIndex)
{
    outState = TransformHierarchyState{};
    outIndex = 0;

    if (!transformAddress)
    {
        return false;
    }

    std::uintptr_t statePtr = 0;
    if (!ReadPtr(mem, transformAddress + off.state_ptr, statePtr) || !statePtr)
    {
        return false;
    }

    if (!ReadPtr(mem, statePtr + off.state_node_data, outState.nodeData))
    {
        return false;
    }

    if (!ReadPtr(mem, statePtr + off.state_parent_indices, outState.parentIndices))
    {
        return false;
    }

    if (!ReadValue(mem, transformAddress + off.index, outIndex))
    {
        return false;
    }

    if (!outState.nodeData || !outState.parentIndices || outIndex < 0)
    {
        return false;
    }

    return true;
}

// SIMD quaternion rotation: v' = v + 2*w*t + 2*cross(q, t), where t = cross(q, v)
inline __m128 QuatRotateSIMD(__m128 q, __m128 v)
{
    // q = [qx, qy, qz, qw]
    // v = [vx, vy, vz, 0]
    
    // q_yzx = [qy, qz, qx, qw]
    // q_zxy = [qz, qx, qy, qw]
    const __m128 q_yzx = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 0, 2, 1));
    const __m128 q_zxy = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 1, 0, 2));
    
    // v_yzx = [vy, vz, vx, 0]
    // v_zxy = [vz, vx, vy, 0]
    const __m128 v_yzx = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1));
    const __m128 v_zxy = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 1, 0, 2));
    
    // t = cross(q.xyz, v) = q_yzx * v_zxy - q_zxy * v_yzx
    const __m128 t = _mm_sub_ps(_mm_mul_ps(q_yzx, v_zxy), _mm_mul_ps(q_zxy, v_yzx));
    
    // t_yzx, t_zxy for cross(q, t)
    const __m128 t_yzx = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 2, 1));
    const __m128 t_zxy = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 1, 0, 2));
    
    // cross_qt = cross(q.xyz, t) = q_yzx * t_zxy - q_zxy * t_yzx
    const __m128 cross_qt = _mm_sub_ps(_mm_mul_ps(q_yzx, t_zxy), _mm_mul_ps(q_zxy, t_yzx));
    
    // w = qw broadcast to all lanes
    const __m128 w = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
    
    // result = v + 2 * (w * t + cross_qt)
    const __m128 two = _mm_set1_ps(2.0f);
    const __m128 wt = _mm_mul_ps(w, t);
    const __m128 inner = _mm_add_ps(wt, cross_qt);
    const __m128 result = _mm_add_ps(v, _mm_mul_ps(two, inner));
    
    return result;
}

inline bool ComputeWorldPositionFromHierarchy(
    const IMemoryAccessor& mem,
    const TransformOffsets& off,
    const TransformHierarchyState& state,
    std::int32_t index,
    glm::vec3& outPos,
    int maxDepth)
{
    outPos = glm::vec3(0.0f);

    if (!state.nodeData || !state.parentIndices || index < 0)
    {
        return false;
    }

    // Read self node data (48 bytes: pos[3], pad, quat[4], scale[3], pad)
    float selfNode[12] = {};
    const std::uintptr_t selfAddr = state.nodeData + static_cast<std::uintptr_t>(index) * static_cast<std::uintptr_t>(off.node_stride);
    if (!mem.Read(selfAddr, selfNode, sizeof(selfNode)))
    {
        return false;
    }

    // Accumulated position starts from self local position
    __m128 acc = _mm_loadu_ps(selfNode);

    // Read parent index
    int parent = 0;
    const std::uintptr_t parentAddr0 = state.parentIndices + static_cast<std::uintptr_t>(index) * sizeof(std::int32_t);
    if (!mem.Read(parentAddr0, &parent, sizeof(parent)))
    {
        return false;
    }

    int depth = 0;
    while (parent >= 0 && depth < maxDepth)
    {
        // Read parent node data
        float node[12] = {};
        const std::uintptr_t nodeAddr = state.nodeData + static_cast<std::uintptr_t>(parent) * static_cast<std::uintptr_t>(off.node_stride);
        if (!mem.Read(nodeAddr, node, sizeof(node)))
        {
            return false;
        }

        const __m128 t = _mm_loadu_ps(node + 0);      // translation
        const __m128 q = _mm_loadu_ps(node + 4);      // quaternion (x, y, z, w)
        const __m128 s = _mm_loadu_ps(node + 8);      // scale

        // Transform: acc = t + rotate(q, acc * s)
        const __m128 scaled = _mm_mul_ps(acc, s);
        const __m128 rotated = QuatRotateSIMD(q, scaled);
        acc = _mm_add_ps(t, rotated);

        // Read next parent
        const std::uintptr_t parentAddr = state.parentIndices + static_cast<std::uintptr_t>(parent) * sizeof(std::int32_t);
        if (!mem.Read(parentAddr, &parent, sizeof(parent)))
        {
            return false;
        }
        
        ++depth;
    }

    // Extract result
    float tmp[4];
    _mm_storeu_ps(tmp, acc);
    outPos.x = tmp[0];
    outPos.y = tmp[1];
    outPos.z = tmp[2];
    return true;
}

inline bool GetTransformWorldPosition(
    const IMemoryAccessor& mem,
    const TransformOffsets& off,
    std::uintptr_t transformAddress,
    glm::vec3& outPos,
    int maxDepth = 50)
{
    TransformHierarchyState state;
    std::int32_t index = 0;
    if (!ReadTransformHierarchyState(mem, transformAddress, off, state, index))
    {
        return false;
    }

    return ComputeWorldPositionFromHierarchy(mem, off, state, index, outPos, maxDepth);
}

} // namespace er6
