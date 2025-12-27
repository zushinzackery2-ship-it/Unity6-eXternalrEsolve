# GOM 遍历与搜索算法

## 游戏对象枚举（EnumerateGameObjects）

流程：
- `gomGlobalSlot -> manager`
- `manager -> buckets`（通过 `GetAllLinkedBucketsFromManager` 收集可用 bucket）
- 对每个 bucket：
  - `bucket -> listHead`（两级解引用）
  - `node = listHead`（head 就是第一个节点）
  - 用 `visited` 防循环
  - 读取 `node -> nativeObject`
  - 可选读取 `nativeObject + off.game_object.managed -> managedObject`
  - 推入 `GameObjectEntry{node,nativeObject,managedObject}`
  - `next = node.next`，遇到 `next==listHead` 结束

## GomWalker 组件枚举（EnumerateComponents）

流程：
- 先 `EnumerateGameObjects`
- 对每个 `nativeObject`：
  - `GameObject + component_pool -> pool`
  - `GameObject + component_count -> count`（要求 1..1024）
  - 对每个 slot：
    - `slot.native -> nativeComponent`
    - `nativeComponent + off.component.managed -> managedComponent`
    - 推入 `ComponentEntry{nativeComponent,managedComponent}`

## 游戏对象（GameObject）搜索

### FindGameObjectThroughTag

- `FindBucketThroughTag(manager, tag)`
- `bucket -> listHead`
- 遍历 list，读取每个 `nativeObject` 的 tag：
  - `GameObject + off.game_object.tag_raw -> raw`
  - `tagValue = raw & 0xFFFF`
- 命中即返回。

### FindGameObjectThroughName

- 先枚举所有 gameObjects
- `ReadNativeGameObjectName(GameObject + off.game_object.name_ptr -> cstring)`
- 字符串相等则返回。

### FindGameObjectsThroughNameContainsAll

- 对每个 gameObjectName，要求包含所有子串 `parts`。

## 组件搜索（Component）

### FindComponentsOnGameObjectThroughClassNameContainsAll

前置条件：`Supports(backend, Feature::ManagedClassName)`

- 遍历 GameObject 的组件 slot
- `nativeComp -> managedComp`
- `ReadManagedObjectClassName(managedComp) -> className`
- `ContainsAllSubstrings(className, parts)` 命中则收集 nativeComp

### GetComponentThroughTypeId

- 遍历组件 slot
- `slot.typeId == typeId` 时返回 `slot.native`

### GetComponentThroughTypeName

- 遍历 slot
- `nativeComp -> managedComp`
- `ReadManagedObjectTypeInfo(managedComp) -> TypeInfo`
- `TypeInfo.name == typeName` 命中返回

### FindGameObjectWithComponent

- 枚举所有 GameObject
- 对每个 go 调用 `GetComponentThroughTypeName`，命中返回 go
