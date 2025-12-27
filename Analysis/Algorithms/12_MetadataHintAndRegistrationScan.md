# Metadata Hint 导出 + IL2CPP Registration 扫描

## 元数据提示（MetadataHint）JSON 生成流程

### BuildMetadataHintJson

- 将 `MetadataHint` 的 wstring 字段转 UTF-8（`WideToUtf8`）
- 做 JSON 转义（`JsonEscape`）
- 输出 JSON：
  - process：name/pid
  - module：name/base/size/path/pe_image_base
  - metadata：s_global_metadata_addr/meta_base/total_size/magic/version/images_count/assemblies_count
  - il2cpp：code_registration/metadata_registration 及其 RVA

## 提示（Hint）构建与 Sidecar 输出流程

### BuildMetadataHintImpl

流程：
- 可选：查询模块路径 `TryQueryModulePath(pid, moduleName)`
- `ReadModuleSections` 得到 moduleSize
- `ReadModuleImageBase` 得到 peImageBase
- `FindMetadataPointerByScore` 得到 `metaBase` 与 `sGlobalMetadataAddr`
- `CalcTotalSizeFromHeader` 得到 totalSize
- 读取 header 的 magic/version、imagesCount、assembliesCount
- 若 modulePath 有效：调用 `FindIl2CppRegistrations` 扫描 code/meta registration，并计算 RVA

### ExportMetadataHintJsonTScore / TVersion

- 调用 BuildMetadataHintImpl
- 再 `BuildMetadataHintJson` 输出字符串

### ExportMetadataHintJsonTScoreToSidecar

- 输出到 sidecar hint json

## IL2CPP 注册表扫描入口

`FindIl2CppRegistrations`：
- `FindCodeRegistration(...) -> cr`
- `FindMetadataRegistration(...) -> mr`
- 返回 `Il2CppRegs{cr,mr}`

## 代码注册（CodeRegistration）扫描流程

核心思路：
- 读 metadata header 得到 `imagesCount`
- 根据 `version` 调用 `GetCodeRegistrationOffsets(version)`
- 扫描 `.data/.rdata` 等 section 的内存块：
  - 读取候选基址 baseAddr
  - 检查 `codeGenModulesCount == imagesCount`
  - 校验 `codeGenModules` 数组指针指向模块内，并且 moduleName 合理
  - 校验 `invokerPointersCount` 合理，且 `invokerPointers` 数组指向 `.text` 可执行范围

命中则 `outAddr = baseAddr`。

## 元数据注册（MetadataRegistration）扫描流程

核心思路：
- 根据 version 得到 `MetaRegOffsets`
- 从 metadata header 读取 `typeDefinitionsSizes` 总大小，推导可能的 typeDefCount 候选集合
- 扫描数据段：
  - 校验 `fieldOffsetsCount == typeDefinitionsSizesCount` 并落在候选集合
  - `typesCount` 合理
  - `types/fieldOffsets/typeSizes` 指针都落在 dataRanges
  - 对 `types`、`fieldOffsets` 做少量采样读指针验证

命中则 `outAddr = baseAddr`。

## 对应工具/调用点

- `dump_metadata6`：导出 sidecar hint json
- `dump_methods6`：构建 hint（包含 codegen modules 列表）并用于方法地址解析
- `dump_sdk6`：DumpSDK6 流程内部会构建 hint，并可选择输出 sidecar hint json
