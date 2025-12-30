# GOM 搜索通用结构

## 概述

GOM 搜索模块的通用工具函数和封装。

## 字符串匹配

### ContainsAllSubstrings

```
输入:
  - text: 待搜索字符串
  - parts: 子串列表

输出:
  - bool: 是否包含所有子串

规则:
- parts 为空返回 false
- 空子串被跳过
- 所有非空子串都必须在 text 中找到
```

## 枚举封装

### EnumerateGameObjects

```
输入:
  - mem: IMemoryAccessor
  - gomGlobalSlot: GOM 全局槽地址
  - gomOff: GomOffsets

输出:
  - out: vector<GameObjectEntry>

封装:
  创建 GomWalker 并调用 EnumerateGameObjects
```

### EnumerateComponents

```
输入:
  - mem: IMemoryAccessor
  - gomGlobalSlot: GOM 全局槽地址
  - gomOff: GomOffsets

输出:
  - out: vector<ComponentEntry>

封装:
  创建 GomWalker 并调用 EnumerateComponents
```

### GetAllLinkedBuckets

```
输入:
  - mem: IMemoryAccessor
  - gomGlobalSlot: GOM 全局槽地址
  - off: GomOffsets

输出:
  - outBuckets: vector<uintptr_t>

步骤:
1. 从 gomGlobalSlot 读取 manager 地址
2. 调用 GetAllLinkedBucketsFromManager
```

## 使用模式

```
// 枚举所有 GameObject
vector<GameObjectEntry> gameObjects;
EnumerateGameObjects(mem, gomSlot, off, gameObjects);

// 枚举所有 Component
vector<ComponentEntry> components;
EnumerateComponents(mem, gomSlot, off, components);

// 获取所有有效桶
vector<uintptr_t> buckets;
GetAllLinkedBuckets(mem, gomSlot, off, buckets);
```
