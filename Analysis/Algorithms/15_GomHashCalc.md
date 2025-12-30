# GOM 哈希计算算法

## 目的

根据 GameObject 的 tag 值计算对应的哈希掩码（hashmask），用于在 GameObjectManager 的桶数组中定位。

## 算法

### CalHashmaskThrougTag

```
输入:
  - tagId: int32 (GameObject 的 tag 值)

输出:
  - hashMask: uint32 (哈希掩码，低 2 位清零)

步骤:
1. searchKey = (uint32)tagId
2. t = 4097 * searchKey + 2127912214
3. hash1 = t ^ (t >> 19) ^ 0xC761C23C
4. x = 33 * hash1
5. y = 9 * (((x + 374761393) << 9) ^ (x - 369570787)) - 42973499
6. finalHash = y ^ (y >> 16) ^ 0xB55A4F09
7. hashMask = finalHash & 0xFFFFFFFC  // 清除低 2 位
8. 返回 hashMask
```

## 用途

- 通过 tag 快速定位 bucket：`FindBucketThroughTag`
- 验证 bucket 的 hashmask 与 key 是否一致：`IsBucketHashmaskKeyConsistent`

## 哈希特性

- 输入：32 位有符号整数（tag）
- 输出：32 位无符号整数，低 2 位始终为 0
- 单向映射：tag → hashmask
- 用于桶索引时需配合桶数量取模
