# Metadata Strings 结构

## 概述

IL2CPP 元数据中的字符串表，存储所有类名、方法名、命名空间等字符串。

## 字符串读取

### ReadCStringFromMetadataStrings (内存版)

```
输入:
  - mem: IMemoryAccessor
  - metaBase: 元数据基址
  - h: MetadataHeaderFields
  - nameIndex: 字符串索引

输出:
  - out: string

步骤:
1. 验证 nameIndex >= 0
2. 验证 nameIndex < h.stringSize
3. 计算地址: p = metaBase + h.stringOffset + nameIndex
4. 从 p 读取 C 字符串 (最大 260 字符)
```

### ReadCStringFromMetadataStringsBytes (字节数组版)

```
输入:
  - meta: 元数据字节数组
  - h: MetadataHeaderFields
  - nameIndex: 字符串索引

输出:
  - out: string

步骤:
1. 验证索引范围
2. 计算起始位置: start = h.stringOffset + nameIndex
3. 查找 '\0' 终止符
4. 提取字符串
```

## 内存布局

```
Metadata
+h.stringOffset --> "ClassName1\0"
                    "MethodName\0"
                    "Namespace\0"
                    ...
+h.stringOffset + h.stringSize (结束)
```

## 索引规则

- nameIndex 是相对于 stringOffset 的字节偏移
- 字符串以 '\0' 结尾
- 索引 0 通常指向空字符串或第一个有效字符串

## 用途

- 读取类型名称
- 读取方法名称
- 读取命名空间
- 读取程序集名称
