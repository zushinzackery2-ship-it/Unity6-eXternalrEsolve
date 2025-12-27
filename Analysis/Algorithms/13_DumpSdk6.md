# DumpSDK6 导出流程

## 目标

SDK 导出的目标是从目标进程导出：

- C# API 描述
- 泛型相关结构描述

输出目录由 dumpsdk 内部路径工具生成（工具侧只接收 `DumpSdk6Paths`）。

## 调用入口

流程非常薄：

- `FindUnityWndClassPids()`
- `DumpSdk6DumpByPid(pid, paths)`
- 输出 `paths` 中记录的产物路径

## dumpsdk 模块拆分

- `sdk_runner`
  - `DumpSdk6DumpByPid`：串起完整导出流程
- `path`
  - 输出路径相关（exe 目录、JoinPath、DumpSDK 目录）
- `sdk_registration`
  - 从 `metadataRegistration` 读取 `types` 数组指针与数量
- `sdk_metadata_helpers`
  - 从 metadata bytes 读取字符串
  - 泛型参数索引/名称构建
  - MethodDef 布局探测与读取
- `sdk_type_resolver`
  - 递归解析 `Il2CppType`，生成可读类型名（含泛型、数组等）
- `sdk_dump_cs`
  - 生成 C# API 描述
- `sdk_generic_json`
  - 生成泛型相关结构描述

## 依赖的 metadata 导出与 hint

dumpsdk 的 metadata bytes 依赖：

- `ExportMetadataByScore`

hint（可用于方法地址/模块信息）依赖：

- `BuildMetadataHintTScore` / `ExportMetadataHintJsonTScoreToSidecar`

