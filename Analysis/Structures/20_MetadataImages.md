# Metadata Images 结构

## 概述

IL2CPP 元数据中的 Image 表，记录每个程序集（Assembly）的类型定义范围。

## MetadataImageInfo

```
struct MetadataImageInfo
{
    index     : uint32   // Image 索引
    name      : string   // 程序集名称 (如 "Assembly-CSharp")
    typeStart : uint32   // 类型定义起始索引
    typeCount : uint32   // 类型定义数量
}
```

## 原始布局

```
Il2CppImageDefinition (stride = 0x28)
+0x00  nameIndex (int32)    // 字符串表索引
+0x08  typeStart (int32)    // TypeDefinition 起始索引
+0x0C  typeCount (int32)    // TypeDefinition 数量
+0x10  ...                  // 其他字段
```

## 读取算法

### ReadImagesFromMemory / ReadImagesFromBytes

```
输入:
  - metaBase: 元数据基址
  - h: MetadataHeaderFields

输出:
  - out: vector<MetadataImageInfo>

步骤:
1. 验证 imagesOffset 和 imagesSize
2. 计算 count = imagesSize / 0x28
3. 对于每个 image:
   a. 读取 nameIndex, typeStart, typeCount
   b. 从字符串表读取 name
   c. 收集到结果列表
```

### BuildTypeDefIndexToImageNameMap

```
输入:
  - images: vector<MetadataImageInfo>

输出:
  - outTypeToImage: vector<string>

用途:
  建立 TypeDefinition 索引到 Image 名称的映射表。
  outTypeToImage[typeDefIndex] = imageName
```

## 内存布局

```
Metadata
+h.imagesOffset --> Image[0] (0x28 bytes)
                    Image[1]
                    ...
                    Image[count-1]
```

## 用途

- 确定类型属于哪个程序集
- SDK 导出时按程序集分组
- 过滤特定程序集的类型
