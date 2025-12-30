# 骨骼获取算法 - Transform 子树遍历

## 概述

Unity6 中获取骨骼的标准方法，通过遍历 Transform 子树实现，不依赖 Animator 组件。

## 目的

给定任意角色的根 `GameObject/Transform`，枚举其 Transform 子树内的节点（骨骼与辅助节点），输出：

- 节点数量
- 节点索引（BoneIndex / TransformHierarchy index）
- 节点名（GameObject name）
- 节点世界坐标

## 算法

### GetBoneTransformAll

```
输入:
  - rootGameObjectNative: Native GameObject 指针
  - maxDepth: 最大遍历深度 (默认 10)
  - maxNodes: 最大节点数 (默认 4096)

输出:
  - vector<BoneTransformAllItem>

步骤:
1. rootTransform = GetTransformComponent(rootGameObject)
2. 初始化栈: stack = [{rootTransform, depth=0}]
3. 初始化访问集合: visited = {}
4. while stack 非空:
   a) 弹出 {tr, depth}
   b) 跳过: depth > maxDepth / 无效指针 / 已访问
   c) 标记已访问
   d) 读取 tr 对应的 GameObject
   e) 读取 Transform 层级索引
   f) 读取 GameObject 名称
   g) 收集 BoneTransformAllItem{index, boneName, transform}
   h) 读取子节点:
      - tr+0x60 -> childrenPtr
      - tr+0x70 -> childrenCount
   i) 将子节点压入栈
5. 返回结果列表
```

## 相关结构与偏移（Unity6）

### Transform 子节点

```
Transform+0x60 -> childrenPtr (指向子 Transform 指针数组)
Transform+0x70 -> childrenCount (子节点数量)
```

### TransformHierarchy

```
Transform+0x28 -> state_ptr
Transform+0x30 -> index

state_ptr+0x18 -> nodeData
state_ptr+0x20 -> parentIndices

node_stride = 48
```

### NativeComponent -> GameObject

```
NativeComponent+0x30 -> game_object
```

### NativeGameObject

```
GameObject+0x50 -> name_ptr
```

## 备注

- 该方法枚举的是 Transform 子树节点，可能包含：骨骼、碰撞体节点、挂点、装备节点等
- 若需要"只保留骨骼"，应在输出阶段增加过滤规则（按命名约定/组件类型/层级路径）
- 此方法不依赖 Animator 组件，适用于所有 GameObject

## 对外 API（init 层）

```cpp
// 获取所有骨骼信息
GetBoneTransformAll(rootGo, maxDepth, maxNodes) -> vector<BoneTransformAllItem>

// BoneTransformAllItem 结构
struct BoneTransformAllItem {
    int32_t index;           // Transform 层级索引
    string boneName;         // GameObject 名称
    uintptr_t transform;     // Native Transform 指针
};

// 获取单个骨骼世界坐标
GetTransformWorldPosition(transform) -> optional<vec3>
```
