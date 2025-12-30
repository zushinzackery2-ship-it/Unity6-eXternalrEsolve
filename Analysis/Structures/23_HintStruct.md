# Hint 结构 - 元数据提示

## 概述

MetadataHint 用于导出 sidecar JSON 文件，记录运行时发现的元数据位置和关键信息。

## CodeGenModuleHint

```
struct CodeGenModuleHint
{
    name               : string     // 模块名称 (Il2CppCodeGenModule::moduleName)
    address            : uintptr_t  // Il2CppCodeGenModule 结构地址
    methodPointerCount : uint32     // 方法指针数量
    methodPointers     : uintptr_t  // 方法指针数组地址
}
```

## MetadataHint

```
struct MetadataHint
{
    // 版本
    schema : uint32 = 1

    // 进程信息
    pid         : uint32
    processName : wstring

    // 模块信息
    moduleName  : wstring
    modulePath  : wstring
    moduleBase  : uintptr_t
    moduleSize  : uint32
    peImageBase : uint64

    // 元数据位置
    sGlobalMetadataAddr : uintptr_t   // s_GlobalMetadata 全局变量地址
    metaBase            : uintptr_t   // 元数据实际基址
    totalSize           : uint32      // 元数据总大小
    magic               : uint32      // 魔数 (0xFAB11BAF)
    version             : uint32      // 元数据版本

    // 统计
    imagesCount     : uint32
    assembliesCount : uint32

    // IL2CPP 注册信息
    codeRegistration     : uintptr_t
    metadataRegistration : uintptr_t
    codeRegistrationRva  : uint64
    metadataRegistrationRva : uint64

    // CodeGen 模块
    codeGenModulesCount : uint32
    codeGenModules      : uintptr_t   // Il2CppCodeGenModule*[] 地址
    codeGenModuleList   : vector<CodeGenModuleHint>
}
```

## 用途

- 导出运行时发现的地址信息
- 供离线分析工具使用
- 跨会话复用扫描结果
- 调试和验证

## JSON 输出示例

```json
{
  "schema": 1,
  "pid": 12345,
  "processName": "Game.exe",
  "moduleBase": "0x7FF600000000",
  "metaBase": "0x1A2B3C4D5E6F",
  "version": 31,
  "codeRegistration": "0x...",
  "metadataRegistration": "0x...",
  "codeGenModuleList": [
    {
      "name": "Assembly-CSharp",
      "address": "0x...",
      "methodPointerCount": 5000,
      "methodPointers": "0x..."
    }
  ]
}
```
