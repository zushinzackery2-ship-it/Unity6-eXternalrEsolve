# GOM Bucket 结构 - 哈希桶

## 概述

Bucket 是 GameObjectManager 哈希表的基本单元。
每个 bucket 通过 hashmask 和 key (tag) 索引，指向一个 GameObject 链表。

## 桶操作

### GetBucketValue

```
输入:
  - bucketPtr: 桶指针
  - off: GomOffsets

输出:
  - outValue: 中间指针

读取:
  bucketPtr + off.bucket.value -> outValue
```

### GetBucketListHead

```
输入:
  - bucketPtr: 桶指针
  - off: GomOffsets

输出:
  - outListHead: 链表头指针

步骤:
1. GetBucketValue -> intermediate
2. intermediate + 0x00 -> outListHead
```

### GetBucketHashmask / GetBucketKey

```
读取:
  bucketPtr + off.bucket.hash_mask -> outMask (uint32)
  bucketPtr + off.bucket.key -> outKey (uint64)
```

### IsBucketHashmaskKeyConsistent

```
验证:
  expected = CalHashmaskThrougTag((int32)key)
  return mask == expected
```

## 内存布局

```
Bucket (stride = 0x18)
+0x00  hash_mask (uint32)
+0x08  key (uint64, 低 32 位为 tag)
+0x0C  flags
+0x10  value -----> intermediate
                    |
                    +0x00 --> listHead (ListNode 链)
```

## 桶查找

### FindBucketThroughTag

```
输入:
  - manager: GameObjectManager 指针
  - tagId: 目标 tag

步骤:
1. 获取 bucketsBase 和 bucketCount
2. 遍历所有桶
3. 读取桶的 key，比较低 32 位与 tagId
4. 匹配则返回桶指针
```

### FindBucketThroughHashmask

```
输入:
  - manager: GameObjectManager 指针
  - hashMask: 目标哈希掩码

步骤:
1. 遍历所有桶
2. 读取桶的 hash_mask
3. 验证 hashmask 与 key 一致性
4. 匹配则返回桶指针
```

### GetAllLinkedBucketsFromManager

```
输入:
  - manager: GameObjectManager 指针

输出:
  - outBuckets: 有效桶指针列表

步骤:
1. 遍历所有桶
2. 验证 hashmask-key 一致性
3. 验证 listHead 非空
4. 收集有效桶
```
