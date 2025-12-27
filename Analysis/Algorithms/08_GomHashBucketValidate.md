# GOM Hash / Bucket / 候选校验算法

## Hashmask 哈希掩码计算

`CalHashmaskThrougTag(tagId)`：
- 使用一套固定的整数混合（乘法、异或、右移）生成 `finalHash`
- `hashMask = finalHash & 0xFFFFFFFC`

用途：校验 bucket 的 `hash_mask` 与 `key(tag)` 是否一致。

## 桶一致性校验

`IsBucketHashmaskKeyConsistent`：
- 读 `bucket.hash_mask -> mask`
- 读 `bucket.key -> key`
- `expected = CalHashmaskThrougTag((int32)(uint32)key)`
- `mask == expected` 才认为该 bucket 可信

## listHead 两级解引用说明

- `bucket.value -> intermediate`
- `intermediate+0x00 -> listHead`

## GOM 扫描候选校验

### ValidateCircularDList

验证 `listHead` 是否是有效的循环双向链表：
- 读 `head+0x00 -> headPrev`、`head+0x08 -> headNext`
- 检查 `headNext.prev == head`、`headPrev.next == head`
- 使用快慢指针检测是否存在环，并确认 `head` 在环内
- 最后沿 next 遍历，逐节点检查 prev/next 互相一致

输出：
- `outSteps`：从 `headNext` 开始走到回到 head 的步数

### CheckGameObjectManagerCandidateBlindScan

输入：`manager` 指针候选

主要检查：
- `manager.local_game_object_list_head` 存在且循环链表有效（得到 listSteps）
- buckets 指针有效
- bucketCount 合理
- 扫描前 1000 个 bucket：
  - `hashmask/key` 一致
  - `GetBucketListHead` 成功
  - `IsListNonEmpty`（listHead 节点能读到合理 nativeObject）

评分：
- 固定项 +20/+80/+20
- `listSteps` 上限 64
- `validBucketCount` 上限 64
