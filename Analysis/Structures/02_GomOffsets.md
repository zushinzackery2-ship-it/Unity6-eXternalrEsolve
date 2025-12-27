# GOM Offsets - GameObjectManager 结构

## 概述

GOM (GameObjectManager) 使用哈希桶结构管理场景中的所有 GameObject。

## GOM 偏移表（GomOffsets）

```
struct GomManagerOffsets
{
    buckets_ptr                = 0x00   // Manager -> 桶数组指针
    bucket_count               = 0x08   // Manager -> 桶数量
    local_game_object_list_head = 0x18  // Manager -> 本地列表头
}

struct GomBucketOffsets
{
    hash_mask  = 0x00   // Bucket -> 哈希掩码
    key        = 0x08   // Bucket -> tag 键 (uint64)
    flags      = 0x0C   // Bucket -> 标志
    value      = 0x10   // Bucket -> 中间指针
    stride     = 24     // Bucket 大小 (0x18)
}

struct GomListNodeOffsets
{
    next          = 0x08   // Node -> 下一节点指针
    native_object = 0x10   // Node -> Native GameObject 指针
}

struct NativeGameObjectOffsets
{
    managed         = 0x18   // GameObject -> 托管对象指针
    component_pool  = 0x20   // GameObject -> 组件池指针
    component_count = 0x30   // GameObject -> 组件数量
    tag_raw         = 0x44   // GameObject -> tag 值
    name_ptr        = 0x50   // GameObject -> 名称字符串指针
}

struct NativeComponentOffsets
{
    managed     = 0x18   // Component -> 托管对象指针
    game_object = 0x30   // Component -> 所属 GameObject 指针
    enabled     = 0x38   // Component -> 启用状态
}

struct ComponentPoolOffsets
{
    slot_stride  = 0x10   // Slot 大小 (16 字节)
    slot_type_id = 0x00   // Slot -> 类型 ID
    slot_native  = 0x08   // Slot -> Native 组件指针
}
```

## 内存布局图

### GameObjectManager
```
GlobalSlot (.data 段中)
    |
    v
+0x00  buckets_ptr -----> Bucket 数组
+0x08  bucket_count
+0x18  local_list_head --> ListNode 链
```

### Bucket 结构
```
Bucket[i] (stride = 0x18)
+0x00  hash_mask
+0x08  key (tag 值)
+0x0C  flags
+0x10  value -----> intermediate
                    |
                    +0x00 --> listHead (第一个 ListNode)
```

### ListNode 链
```
ListNode
+0x08  next ---------> 下一个 ListNode (或 listHead 表示结束)
+0x10  native -------> NativeGameObject
```

### NativeGameObject
```
+0x00  vtable
+0x18  managed ------> 托管 GameObject
+0x20  component_pool -> ComponentPool
+0x30  component_count
+0x44  tag_raw
+0x50  name_ptr -----> "ObjectName"
```

### ComponentPool
```
Slot[i] (stride = 0x10)
+0x00  type_id (int32)
+0x08  native -------> NativeComponent
```
