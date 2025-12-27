# 对象枚举算法 - MSID 遍历

## 目的

 MSID 表是 `UnityEngine.Object` 的全局注册表（InstanceID -> Native Object*）。
 当前算法实现仅从 MSID 中筛出 `UnityEngine.GameObject` 与 `UnityEngine.ScriptableObject`，
 提取它们的名称、类型和地址。

## 算法

### EnumerateMsIdToPointerObjects

```
输入:
  - mem: IMemoryAccessor
  - msIdToPointerAddr: 槽虚拟地址
  - off: Offsets
  - unityPlayer: UnityPlayerRange
  - opt: EnumerateOptions
  - cb: 回调函数(ObjectInfo)

输出:
  - bool 成功

步骤:
1. 从槽读取 MsIdToPointerSet
2. 读取条目头 (entriesBase, count)
3. total = count + 8 (哈希冲突缓冲)
4. 对于 i = 0 到 total:
   a. 计算条目地址 = entriesBase + i * stride
   b. 读取 MsIdToPointerEntryRaw (24 字节)
   c. 如果 key == 0 或 key >= 0xFFFFFFFE 则跳过
   d. obj = entry.object
   e. 如果不是 IsProbablyUnityObject(obj) 则跳过
   f. 从托管对象链读取 klass
   g. 检查是否 IsClassOrParent(klass, "UnityEngine.GameObject")
   h. 检查是否 IsClassOrParent(klass, "UnityEngine.ScriptableObject")
   i. 如果都不是则跳过（MSID 中可能存在其它 `UnityEngine.Object` 子类）
   j. 应用选项中的类型过滤
   k. 读取对象名称:
      - GameObject: offset 0x50
      - ScriptableObject: offset 0x38
   l. 读取 Il2CppClass 名称和命名空间
   m. 构建 typeFullName
   n. 应用字符串过滤
   o. 构建 ObjectInfo 并调用回调
5. 返回 true
```

### IsProbablyUnityObject

```
检查:
1. obj 是规范用户指针
2. obj 是 8 字节对齐
3. vtable 在 UnityPlayer 范围内
4. managed 指针是规范的
```

### ReadUnityObjectKlass

```
链:
  obj+0x18 --> managed
  managed+0x00 --> gchandle
  gchandle+0x00 --> klass
```

### IsClassOrParent

```
步骤:
1. cur = klass
2. 遍历继承链 (最多 64 层):
   a. 如果类名匹配目标，返回 true
   b. cur = parent
3. 返回 false
```
