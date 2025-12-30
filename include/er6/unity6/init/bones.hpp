#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "context.hpp"
#include "component.hpp"
#include "object.hpp"

#include "../object/native/native_component.hpp"
#include "../transform/transform.hpp"

namespace er6
{

struct BoneTransformAllItem
{
    std::int32_t index = 0;
    std::string boneName;
    std::uintptr_t transform = 0;
};

inline std::vector<BoneTransformAllItem> GetBoneTransformAll(std::uintptr_t rootGameObjectNative, int maxDepth = 10, int maxNodes = 4096)
{
    std::vector<BoneTransformAllItem> out;
    if (!IsInited() || !rootGameObjectNative)
    {
        return out;
    }

    const std::uintptr_t rootTr = GetTransformComponent(rootGameObjectNative);
    if (!rootTr)
    {
        return out;
    }

    if (maxDepth < 0)
    {
        return out;
    }

    if (maxNodes <= 0)
    {
        return out;
    }

    static constexpr std::uint32_t kTransformChildrenPtr = 0x60;
    static constexpr std::uint32_t kTransformChildrenCount = 0x70;

    struct Pending
    {
        std::uintptr_t tr;
        int depth;
    };

    std::vector<Pending> stack;
    stack.reserve(256);
    stack.push_back({rootTr, 0});

    std::unordered_set<std::uintptr_t> visited;
    visited.reserve(4096);

    while (!stack.empty())
    {
        const Pending cur = stack.back();
        stack.pop_back();

        if (cur.depth > maxDepth)
        {
            continue;
        }

        if (!IsCanonicalUserPtr(cur.tr))
        {
            continue;
        }

        if (visited.find(cur.tr) != visited.end())
        {
            continue;
        }
        visited.insert(cur.tr);

        std::uintptr_t curGo = 0;
        if (er6::GetNativeComponentGameObject(Mem(), cur.tr, g_ctx.gomOff, curGo) && curGo)
        {
            BoneTransformAllItem item;
            item.transform = cur.tr;

            TransformHierarchyState st;
            std::int32_t idx = 0;
            if (er6::ReadTransformHierarchyState(Mem(), cur.tr, g_ctx.transformOff, st, idx))
            {
                item.index = idx;
            }

            std::string name;
            if (er6::ReadNativeGameObjectName(Mem(), curGo, g_ctx.gomOff, name))
            {
                item.boneName = std::move(name);
            }

            out.push_back(std::move(item));
            if (static_cast<int>(out.size()) >= maxNodes)
            {
                break;
            }
        }

        std::uintptr_t childrenPtr = 0;
        if (!er6::ReadPtr(Mem(), cur.tr + kTransformChildrenPtr, childrenPtr) || !childrenPtr)
        {
            continue;
        }

        std::uint32_t childrenCount = 0;
        if (!er6::ReadValue(Mem(), cur.tr + kTransformChildrenCount, childrenCount))
        {
            continue;
        }

        if (childrenCount == 0 || childrenCount >= 100)
        {
            continue;
        }

        for (std::uint32_t i = 0; i < childrenCount; ++i)
        {
            std::uintptr_t child = 0;
            if (!er6::ReadPtr(Mem(), childrenPtr + static_cast<std::uintptr_t>(i) * sizeof(std::uintptr_t), child) || !child)
            {
                continue;
            }

            stack.push_back({child, cur.depth + 1});
        }
    }

    return out;
}

} // namespace er6
