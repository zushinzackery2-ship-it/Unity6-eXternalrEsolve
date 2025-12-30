# GOM Walker 类型定义

## 概述

GomWalker 遍历过程中使用的数据结构，用于收集 GameObject 和 Component 信息。

## GameObjectEntry

```
struct GameObjectEntry
{
    node          : uintptr_t   // ListNode 地址
    nativeObject  : uintptr_t   // Native GameObject 地址
    managedObject : uintptr_t   // 托管 GameObject 地址
}
```

用途：
- 存储枚举到的 GameObject 信息
- 同时保留 native 和 managed 引用
- node 用于调试和追踪

## ComponentEntry

```
struct ComponentEntry
{
    nativeComponent  : uintptr_t   // Native Component 地址
    managedComponent : uintptr_t   // 托管 Component 地址
}
```

用途：
- 存储枚举到的 Component 信息
- 同时保留 native 和 managed 引用

## 使用场景

### EnumerateGameObjects

```
输出: vector<GameObjectEntry>

遍历所有 bucket 的 ListNode 链，收集每个节点的：
- node 地址
- nativeObject (node + off.node.native_object)
- managedObject (nativeObject + off.game_object.managed)
```

### EnumerateComponents

```
输出: vector<ComponentEntry>

先枚举所有 GameObject，然后对每个 GameObject：
- 获取 component_pool 和 component_count
- 遍历每个 slot，收集：
  - nativeComponent (slot.native)
  - managedComponent (nativeComponent + off.component.managed)
```
