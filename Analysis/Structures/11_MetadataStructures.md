# Metadata / PE / Hint 相关结构

## PE 段信息

### ModuleSection

```
struct ModuleSection
{
    std::string name;
    std::uint32_t rva;
    std::uint32_t size;
};
```

用途：读取远程模块的节表，用于扫描 `.data/.rdata/...`。

## 元数据扫描结果

### FoundMetadata

```
struct FoundMetadata
{
    std::uintptr_t ptrAddr;   // 指向 metadata 的指针所在地址（例如 s_GlobalMetadata 地址）
    std::uintptr_t metaBase;  // metadata 实际基址
    std::uint32_t maxEnd;     // 通过 header 计算得到的最大结束偏移
    int score;                // 评分
};
```

## 提示（Hint）数据结构

### MetadataHint

用于导出一个 sidecar hint json，记录：

- 进程/模块信息（pid、moduleBase、moduleSize、peImageBase、modulePath）
- metadata 信息（sGlobalMetadataAddr、metaBase、totalSize、magic、version）
- image/assembly 计数（imagesCount、assembliesCount）
- IL2CPP registrations（codeRegistration、metadataRegistration + RVA）

这个结构最终通过 `BuildMetadataHintJson` 序列化为 JSON。
