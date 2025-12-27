# MSID 扫描算法 - MsIdToPointer 发现

## 目的

通过扫描有效的 MSID set 结构，在 UnityPlayer 模块中找到全局 ms_id_to_pointer 槽。
MSID 表是 `UnityEngine.Object` 的全局注册哈希表（InstanceID -> Native Object*）。

## 算法

### FindMsIdToPointerSlotVaByScan

```
输入:
  - mem: IMemoryAccessor
  - unityPlayer: ModuleInfo (base + size)
  - off: Offsets
  - unityPlayerRange: UnityPlayerRange

输出:
  - MsIdToPointer 槽虚拟地址
  - (可选) 找到的最佳对象数量

步骤:
1. 从模块读取 PE 段
2. 过滤出 .data 和 .rdata 段
3. 对于每个段:
   a. 将整个段读入缓冲区
   b. 对于每个 8 字节对齐的偏移:
      i.   读取指针值 (候选 set 指针)
      ii.  如果不是规范用户指针则跳过
      iii. 在指针位置读取 16 字节 (set 头)
      iv.  提取 entriesBase (偏移 0x00) 和 count (偏移 0x08)
      v.   验证 entriesBase 可读
      vi.  验证 count 在范围内 (1-5000000)
      vii. 调用 CountUnityObjectsInMsIdEntriesPool
      viii.如果 objCount > bestObjCount，更新最佳候选
4. 返回最佳候选地址
```

### CountUnityObjectsInMsIdEntriesPool

```
输入:
  - mem: IMemoryAccessor
  - entriesBase: 条目数组指针
  - count: 条目数量
  - off: Offsets
  - unityPlayer: UnityPlayerRange

输出:
  - 找到的有效 Unity 对象数量

步骤:
1. 计算池大小 = count * entry_stride (0x18)
2. 验证池大小 <= 128MB
3. 将整个池读入缓冲区
4. 对于每个条目:
   a. 读取 entry + 0x00 处的 key
   b. 如果 key == 0xFFFFFFFF 或 0xFFFFFFFE 则跳过
   c. 读取 entry + 0x10 处的 object 指针
   d. 如果不是规范指针则跳过
   e. 读取对象的前 0x20 字节
   f. 验证 vtable 在 UnityPlayer 范围内
   g. 读取 object + 0x18 处的 managed 指针
   h. 验证 managed 指针可读
   i. 增加 objCount
5. 返回 objCount

 备注：此计数逻辑只验证“像 UnityEngine.Object 的 Native 对象形态”，不区分具体子类。
 具体筛选 GameObject/ScriptableObject 发生在“对象枚举”步骤。
```

## 验证标准

### 条目验证
```
有效条目:
  - key != 0
  - key != 0xFFFFFFFF
  - key != 0xFFFFFFFE
  - object 是规范用户指针
```

### 对象验证
```
有效对象:
  - vtable 在 UnityPlayer 模块范围内
  - managed 指针是规范的
  - managed 指针可读
```
