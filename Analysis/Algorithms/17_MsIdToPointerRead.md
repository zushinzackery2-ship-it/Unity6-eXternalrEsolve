# MSID 表读取算法

## 目的

从 MSID 全局槽读取 InstanceID 到指针的映射表。

## 算法

### ReadMsIdToPointerSet

```
输入:
  - mem: IMemoryAccessor
  - msIdToPointerAddr: MSID 全局槽地址

输出:
  - out: MsIdToPointerSet

步骤:
1. 从 msIdToPointerAddr 读取指针 setPtr
2. 验证 setPtr 是否为有效用户态指针
3. 返回 MsIdToPointerSet{set = setPtr}
```

### ReadMsIdEntriesHeader

```
输入:
  - mem: IMemoryAccessor
  - set: MsIdToPointerSet
  - off: Offsets

输出:
  - outEntriesBase: uintptr_t (条目数组基址)
  - outCount: uint32_t (条目数量)

步骤:
1. 从 set.set + off.ms_id_set_entries_base 读取 entriesBase
2. 从 set.set + off.ms_id_set_count 读取 count
3. 验证 entriesBase 是否为有效用户态指针
4. 验证 count 在合理范围内 (1..5000000)
5. 返回 entriesBase 和 count
```

## 内存布局

```
GlobalSlot
    |
    v
+0x00 --> MsIdToPointerSetRaw
          +0x00 entriesBase --> Entry[0]
          +0x08 count           Entry[1]
          +0x0C unk0C           ...
```

## 条目遍历

```
for i in 0..(count + 8):
    entry = entriesBase + i * 0x18
    key = entry+0x00 (instanceId)
    object = entry+0x10 (native pointer)
```
