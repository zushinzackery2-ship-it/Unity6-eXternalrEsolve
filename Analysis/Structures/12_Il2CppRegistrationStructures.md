# IL2CPP Registration 结构

## Il2CppRegs 结构说明

```
struct Il2CppRegs
{
    std::uintptr_t codeRegistration;
    std::uintptr_t metadataRegistration;
};
```

## detail_il2cpp_reg 内部结构说明

### DiskSection

用于解析磁盘上的 PE 文件 section table（和内存段对应）：

```
struct DiskSection
{
    char name[9];
    std::uint32_t rva;
    std::uint32_t vsize;
};
```

### CodeRegOffsets

用于根据 metadata version 推导 `Il2CppCodeRegistration` 关键字段偏移（以 8 字节为步长累加）。

关键输出字段：
- `invokerPointersCount`
- `invokerPointers`
- `codeGenModulesCount`
- `codeGenModules`
- `needBytes`（扫描候选结构至少需要的字节数）

### MetaRegOffsets

用于根据 version 生成 `Il2CppMetadataRegistration` 的关键字段偏移。

关键输出字段：
- `typesCount` / `types`
- `fieldOffsetsCount` / `fieldOffsets`
- `typeDefinitionsSizesCount` / `typeDefinitionsSizes`
- `structSize`
