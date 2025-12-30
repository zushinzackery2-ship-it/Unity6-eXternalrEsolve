# Registration Types 结构 - IL2CPP 注册信息

## 概述

IL2CPP 运行时的 CodeRegistration 和 MetadataRegistration 结构，包含方法指针、类型信息等关键数据。

## Il2CppRegs

```
struct Il2CppRegs
{
    codeRegistration     : uintptr_t   // CodeRegistration 结构地址
    metadataRegistration : uintptr_t   // MetadataRegistration 结构地址
}
```

## CodeRegOffsets

```
struct CodeRegOffsets
{
    invokerPointersCount : int   // invokerPointers 数量字段偏移
    invokerPointers      : int   // invokerPointers 数组字段偏移
    codeGenModulesCount  : int   // codeGenModules 数量字段偏移
    codeGenModules       : int   // codeGenModules 数组字段偏移
    needBytes            : int   // 需要读取的最小字节数
}
```

## MetaRegOffsets

```
struct MetaRegOffsets
{
    typesCount               : int   // types 数量字段偏移
    types                    : int   // types 数组字段偏移
    fieldOffsetsCount        : int   // fieldOffsets 数量字段偏移
    fieldOffsets             : int   // fieldOffsets 数组字段偏移
    typeDefinitionsSizesCount: int   // typeDefinitionsSizes 数量字段偏移
    typeDefinitionsSizes     : int   // typeDefinitionsSizes 数组字段偏移
    structSize               : int   // 结构总大小
}
```

## 版本适配

### GetCodeRegistrationOffsets

根据 metadata version 计算字段偏移，支持的版本范围：
- v21-v31+ 各版本字段布局不同
- 某些字段仅在特定版本范围存在

关键字段版本依赖：
- `reversePInvokeWrappers`: v22+
- `codeGenModules`: v24.2+
- `genericAdjustorThunks`: v27.1+
- `unresolvedInstanceCallPointers`: v29.1-v30.99

### GetMetadataRegistrationOffsets

根据 metadata version 计算字段偏移：
- v16 及以下有额外字段
- v19+ 增加新字段

## DiskSection

```
struct DiskSection
{
    name    : char[9]    // 节名称
    rva     : uint32     // 相对虚拟地址
    vsize   : uint32     // 虚拟大小
    rawPtr  : uint32     // 文件偏移
    rawSize : uint32     // 文件大小
}
```

用于 PE 文件解析和 RVA 到文件偏移的转换。
