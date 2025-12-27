# GOM 访问结构

## GOM 管理器读取

- `GetGomManagerFromGlobalSlot(gomGlobalSlot) -> manager`
- `GetGomBucketsPtr(manager + off.manager.buckets_ptr) -> bucketsBase`
- `GetGomBucketCount(manager + off.manager.bucket_count) -> bucketCount`
- `GetGomLocalGameObjectListHead(manager + off.manager.local_game_object_list_head) -> listHead`

## 桶结构访问

### 两级解引用（关键点）

- `bucketPtr + off.bucket.value -> intermediate`
- `intermediate + 0x00 -> listHead`

对应函数：
- `GetBucketValue`：返回 `intermediate`
- `GetBucketListHead`：返回真正的 `listHead`

### Hashmask 与 Key 一致性

- `bucketPtr + off.bucket.hash_mask -> mask`
- `bucketPtr + off.bucket.key -> key`
- `expected = CalHashmaskThrougTag((int32)key)`
- `mask == expected` 才认为该 bucket 可信

## 列表节点访问

- `GetListNodeFirst(listHead)`: 直接返回 `listHead`（listHead 本身就是第一个节点）
- `GetListNodeNext(node + off.node.next) -> next`
- `GetListNodeNative(node + off.node.native_object) -> nativeObject`

## 组件池与槽访问

- `GameObject + off.game_object.component_pool -> pool`
- `GameObject + off.game_object.component_count -> count`

slot 计算：
- `slotBase = pool + index * off.pool.slot_stride`
- `slotBase + off.pool.slot_type_id -> typeId`
- `slotBase + off.pool.slot_native -> nativeComponent`

Component enabled：
- `nativeComponent + off.component.enabled -> uint8 enabled`

## 遍历输出结构

```
struct GameObjectEntry
{
    std::uintptr_t node;
    std::uintptr_t nativeObject;
    std::uintptr_t managedObject;
};

struct ComponentEntry
{
    std::uintptr_t nativeComponent;
    std::uintptr_t managedComponent;
};
```
