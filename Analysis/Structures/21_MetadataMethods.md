# Metadata Methods 结构

## 概述

IL2CPP 元数据中的方法定义表，包含方法名、所属类型和 token。

## MethodDefLayout

```
struct MethodDefLayout
{
    stride      : size_t    // 方法定义结构大小
    tokenOffset : size_t    // token 字段在结构中的偏移
    count       : uint32    // 方法总数
}
```

## MethodDefView

```
struct MethodDefView
{
    index         : uint32   // 方法索引
    nameIndex     : int32    // 方法名在字符串表中的索引
    declaringType : int32    // 所属类型的 TypeDefinition 索引
    token         : uint32   // 方法 token (0x06XXXXXX)
}
```

## 布局检测

### DetectMethodDefLayout

```
输入:
  - meta: 元数据字节数组
  - h: MetadataHeaderFields

输出:
  - outLayout: MethodDefLayout

候选布局:
  - stride=0x20, tokenOffset=0x14 (Unity 6000.2, metadata v31)
  - stride=0x24, tokenOffset=0x18 (旧版本)
  - stride=0x28, tokenOffset=0x1C

检测方法:
1. 对每个候选布局
2. 验证 methodsSize 能被 stride 整除
3. 采样前 256 个方法，检查 token 格式
4. 选择匹配率最高的布局
```

## Token 格式

```
MethodDef token: 0x06XXXXXX
  - 高字节 0x06 表示 MethodDef
  - 低 24 位为方法 RID (Row ID)
```

## 内存布局

```
Metadata
+h.methodsOffset --> MethodDef[0]
                     +0x00 nameIndex
                     +0x04 declaringType
                     +tokenOffset token
                     MethodDef[1]
                     ...
```

## 用途

- 解析方法签名
- 建立方法到类型的映射
- SDK 导出时生成方法声明
