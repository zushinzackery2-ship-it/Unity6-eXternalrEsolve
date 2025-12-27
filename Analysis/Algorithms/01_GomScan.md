# GOM 扫描算法 - GameObjectManager 发现

## 目的

通过扫描有效的 manager 指针模式，在 UnityPlayer 模块的 .data/.rdata 段中
找到全局 GameObjectManager 槽。

## 算法

### FindGomGlobalSlotByScanInternal

```
输入:
  - mem: IMemoryAccessor
  - moduleBase: UnityPlayer 模块基址
  - chunkSize: 扫描块大小
  - off: GomOffsets

输出:
  - GOM 全局槽虚拟地址 (未找到则返回 0)

步骤:
1. 从模块头读取 PE 段
2. 过滤出 .data 和 .rdata 段
3. 对于每个段:
   a. 将整个段读入缓冲区
   b. 对于每个 8 字节对齐的偏移:
      i.   读取指针值
      ii.  如果不是有效用户指针则跳过
      iii. 如果在 .rdata 且指针在模块内则跳过 (静态数据)
      iv.  调用 CheckGameObjectManagerCandidateBlindScan
      v.   如果有效且分数 > bestScore，更新最佳候选
4. 返回最佳候选地址
```

### CheckGameObjectManagerCandidateBlindScan

```
输入:
  - mem: IMemoryAccessor
  - manager: 候选 manager 指针
  - off: GomOffsets

输出:
  - ManagerCandidateCheck { ok: bool, score: int }

验证步骤:
1. 从 manager+0x00 读取 buckets_ptr
2. 从 manager+0x08 读取 bucket_count
3. 验证 bucket_count 在合理范围内 (1-10000)
4. 读取样本桶并验证:
   - key 值是有效 tag
   - value 指针指向有效列表头
   - 列表节点包含有效 native 对象指针
5. 根据找到的有效对象数量计算分数
```

## 评分系统

```
maxScore = 20 + 80 + 64 + 64 + 20 = 248

组成部分:
- 桶结构有效: +20
- 找到有效对象: +80 (按数量缩放)
- 列表遍历成功: +64
- 对象验证: +64
- 一致性检查: +20
```

## 流程图

```
UnityPlayer 模块
       |
       v
+------------------+
| 读取 PE 段       |
+------------------+
       |
       v
+------------------+
| 过滤 .data/.rdata |
+------------------+
       |
       v
+------------------+     +-------------------+
| 扫描每个 8 字节  | --> | 读取指针值        |
| 对齐的偏移       |     +-------------------+
+------------------+            |
       |                        v
       |                 +-------------------+
       |                 | IsLikelyPtr?      |
       |                 +-------------------+
       |                        |
       |                   是   v
       |                 +-------------------+
       |                 | 检查候选          |
       |                 +-------------------+
       |                        |
       |                   ok   v
       |                 +-------------------+
       |                 | 如果分数更高      |
       |                 | 则更新最佳        |
       |                 +-------------------+
       |
       v
+------------------+
| 返回最佳地址     |
+------------------+
```
