# FindObjectsOfTypeAll 算法

## 目的

在 MSID 表中按类名（和可选命名空间）查找所有匹配的 Unity 对象。

## 算法

### FindObjectsOfTypeAll

```
输入:
  - mem: IMemoryAccessor
  - msIdToPointerAddr: MSID 全局槽地址
  - off: Offsets
  - unityPlayer: UnityPlayer 模块范围
  - className: 目标类名
  - nameSpace: 可选命名空间过滤

输出:
  - out: vector<FindObjectsOfTypeAllResult>

步骤:
1. 读取 MsIdToPointerSet
2. 读取 entriesBase 和 count
3. total = count + 8 (额外扫描边界)
4. 对于每个 entry (i = 0..total-1):
   a. 读取 MsIdToPointerEntryRaw
   b. 跳过无效 key (0, 0xFFFFFFFF, >= 0xFFFFFFFE)
   c. 验证 object 是否为有效 Unity 对象
   d. 读取 object 的 klass
   e. 读取 klass 的 namespace 和 className
   f. 如果 className 不匹配则跳过
   g. 如果指定了 nameSpace 且不匹配则跳过
   h. 收集结果 {object, instanceId, slot}
5. 返回结果列表
```

## 结果结构

```
struct FindObjectsOfTypeAllResult
{
    object     : uintptr_t   // Native 对象地址
    instanceId : uint32_t    // Unity 实例 ID
    slot       : uint32_t    // 在 MSID 表中的槽位索引
}
```

## 用途

- 查找场景中所有指定类型的对象
- 类似 Unity 的 `Resources.FindObjectsOfTypeAll<T>()`
- 支持按命名空间精确过滤
