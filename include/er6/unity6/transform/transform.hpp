#pragma once

#include <cstdint>
#include <unordered_set>

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

    if (maxDepth == 0)
    {
        return false;
    }

    float selfNode[12] = {};
    const std::uintptr_t selfAddr = state.nodeData + static_cast<std::uintptr_t>(index) * static_cast<std::uintptr_t>(off.node_stride);
    if (!mem.Read(selfAddr, selfNode, sizeof(selfNode)))
    {
        return false;
    }

    __m128 acc = _mm_loadu_ps(selfNode);

    int parent = 0;
    const std::uintptr_t parentAddr0 = state.parentIndices + static_cast<std::uintptr_t>(index) * sizeof(std::int32_t);
    if (!mem.Read(parentAddr0, &parent, sizeof(parent)))
    {
        return false;
    }

    int depth = 0;
    std::unordered_set<std::int32_t> visited;
    while (parent >= 0)
    {
        if (maxDepth > 0 && depth >= maxDepth)
        {
            return false;
        }

        if (visited.find(parent) != visited.end())
        {
            return false;
        }
        visited.insert(parent);

        float node[12] = {};
        const std::uintptr_t nodeAddr = state.nodeData + static_cast<std::uintptr_t>(parent) * static_cast<std::uintptr_t>(off.node_stride);
        if (!mem.Read(nodeAddr, node, sizeof(node)))
        {
            return false;
        }

        const __m128 t = _mm_loadu_ps(node + 0);
        const __m128 qv = _mm_loadu_ps(node + 4);
        const __m128 m = _mm_loadu_ps(node + 8);

        const __m128 v14 = _mm_mul_ps(m, acc);

        const __m128i qvi = _mm_castps_si128(qv);
        const __m128 v15 = _mm_castsi128_ps(_mm_shuffle_epi32(qvi, 219));
        const __m128 v16 = _mm_castsi128_ps(_mm_shuffle_epi32(qvi, 113));
        const __m128 v17 = _mm_castsi128_ps(_mm_shuffle_epi32(qvi, 142));

        const __m128i v14i = _mm_castps_si128(v14);
        const __m128 v14_x = _mm_castsi128_ps(_mm_shuffle_epi32(v14i, 0));
        const __m128 v14_y = _mm_castsi128_ps(_mm_shuffle_epi32(v14i, 85));
        const __m128 v14_z = _mm_castsi128_ps(_mm_shuffle_epi32(v14i, 170));

        const __m128 two = _mm_set1_ps(2.0f);

        const __m128 q1 = _mm_castsi128_ps(_mm_shuffle_epi32(qvi, 85));
        const __m128 q2 = _mm_castsi128_ps(_mm_shuffle_epi32(qvi, 170));
        const __m128 q0 = _mm_castsi128_ps(_mm_shuffle_epi32(qvi, 0));

        const __m128 part0 = _mm_mul_ps(
            _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(q1, two), v16), _mm_mul_ps(_mm_mul_ps(q2, two), v17)),
            v14_x);

        const __m128 part1 = _mm_mul_ps(
            _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(q2, two), v15), _mm_mul_ps(_mm_mul_ps(q0, two), v16)),
            v14_y);

        const __m128 part2 = _mm_mul_ps(
            _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(q0, two), v17), _mm_mul_ps(_mm_mul_ps(q1, two), v15)),
            v14_z);

        acc = _mm_add_ps(_mm_add_ps(_mm_add_ps(part0, v14), _mm_add_ps(part1, part2)), t);

        const std::uintptr_t parentAddr = state.parentIndices + static_cast<std::uintptr_t>(parent) * sizeof(std::int32_t);
        if (!mem.Read(parentAddr, &parent, sizeof(parent)))
        {
            return false;
        }

        ++depth;
    }

    float tmp[4] = {};
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
    int maxDepth)
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
