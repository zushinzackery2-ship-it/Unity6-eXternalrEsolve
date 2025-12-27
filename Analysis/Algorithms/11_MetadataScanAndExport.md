# IL2CPP Metadata 扫描与导出算法

## 元数据头评分

### ScoreMetadataHeader(buf, off)

核心思路：把 `buf+off` 当成 metadata header，基于多组 (offset,size) 字段做一致性检查并给出 score。

关键检查：
- 对齐：`off % 4 == 0`
- 版本：`version = U32(off+0x04)`
  - `requiredVersion != 0` 必须相等
  - `strictVersion` 时要求版本在合理范围（10..100）
- 必须存在且对齐的 string 区：`stringOffset/stringSize`，且 `stringOffset >= 0x100`、4字节对齐
- images/assemblies：
  - `imagesOffset/imagesSize`、`assembliesOffset/assembliesSize`
  - size 必须是结构体倍数：`imagesSize % 0x28 == 0`，`assembliesSize % 0x40 == 0`
  - count 合理（1..200000）
- 遍历 `kMetadataHeaderPairs`：
  - size==0 允许
  - size!=0 时 offset 必须非 0、对齐、>=0x100
  - 计算 `maxEnd = max(offset+size)`，并要求范围在合理区间

score 组成：
- nonzero 字段数量 * 100000
- maxEnd/0x1000
- stringSize/0x1000
- imagesCount + assembliesCount（限幅）

### CalcTotalSizeFromHeader(metaBase)

读取所有 pairs 的 (offset,size)，计算最大 end 作为 totalSize。

## 指针扫描

### FindMetadataPointerByScore

流程：
- 读取 PE sections，筛选 `.data/.rdata/.pdata/.tls/.reloc`
- 分 chunk 扫描每个 section：
  - 以 8 字节步长读出候选 `ptr`
  - 过滤：非 0、地址范围合理、8 字节对齐
  - 把 `ptr` 按 page（4KB）聚合进 `pageMap[page]`
- 对每个 page：
  - 一次性读 0x1000
  - 对 page 内候选 offset 调用 `ScoreMetadataHeader`
  - 取 score 最大者作为 best
  - 若 best 已找到，尝试读取 `best.metaBase + best.maxEnd - 1` 作为尾部可读性验证

输出：`FoundMetadata{ptrAddr, metaBase, maxEnd, score}`

## 导出流程

### ExportMetadataByScore

- 调用 `FindMetadataPointerByScore` 得到 `metaBase`
- `CalcTotalSizeFromHeader(metaBase) -> totalSize`
- `ReadMetadataRegion(metaBase, totalSize, readChunkSize) -> outBytes`

## 应用场景

- 元数据导出流程：产出 metadata bytes
- 离线解析流程：依赖 metadata bytes 做后续符号/类型分析
- SDK 导出流程：内部同样依赖 `ExportMetadataByScore`
