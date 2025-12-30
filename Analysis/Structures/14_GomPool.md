# GOM Pool 结构 - 组件池

## 概述

ComponentPool 是 GameObject 用于存储其所有组件的数组结构。
每个 slot 包含组件的类型 ID 和 Native 组件指针。

## 池操作

### GetComponentPool

```
输入:
  - gameObject: Native GameObject 指针
  - off: GomOffsets

输出:
  - outPool: 组件池指针

读取:
  gameObject + off.game_object.component_pool -> outPool
```

### GetComponentCount

```
输入:
  - gameObject: Native GameObject 指针
  - off: GomOffsets

输出:
  - outCount: 组件数量 (int32)

读取:
  gameObject + off.game_object.component_count -> outCount
```

### GetComponentSlotTypeId

```
输入:
  - pool: 组件池指针
  - off: GomOffsets
  - index: slot 索引

输出:
  - outTypeId: 组件类型 ID (int32)

计算:
  addr = pool + index * off.pool.slot_stride + off.pool.slot_type_id
  读取 addr -> outTypeId
```

### GetComponentSlotNative

```
输入:
  - pool: 组件池指针
  - off: GomOffsets
  - index: slot 索引

输出:
  - outNative: Native 组件指针

计算:
  addr = pool + index * off.pool.slot_stride + off.pool.slot_native
  读取 addr -> outNative
```

## 内存布局

```
NativeGameObject
+0x20  component_pool -----> Slot[0]
+0x30  component_count      Slot[1]
                            ...
                            Slot[count-1]

Slot (stride = 0x10)
+0x00  type_id (int32)
+0x08  native -------> NativeComponent
```

## 组件启用状态

### GetComponentEnabled

```
输入:
  - nativeComponent: Native 组件指针
  - off: GomOffsets

输出:
  - outEnabled: bool

读取:
  nativeComponent + off.component.enabled -> byte
  outEnabled = (byte != 0)
```
