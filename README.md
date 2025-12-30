<div align="center">

# ExternalResolve6

**Unity6 运行时内存结构算法还原库（er6 / header-only）**

*跨进程读取 | Header-Only | IL2CPP/MONO*

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows%20x64-lightgrey?style=flat-square)
![Header Only](https://img.shields.io/badge/Header--Only-Yes-green?style=flat-square)

</div>

---

> [!CAUTION]
> **免责声明**  
> 本项目仅用于学习研究 Unity 引擎内部结构与算法还原，以及在合法授权前提下的游戏 Modding/插件开发学习与验证，不得用于任何违反游戏服务条款或法律法规的行为。  
> 使用本项目产生的一切后果由使用者自行承担，作者不承担任何责任。  
> 请在合法合规的前提下使用本项目。

> [!NOTE]
> **版本兼容性说明**  
> 本项目面向 Unity6（Windows x64）。对比 Unity202x 版本在结构偏移与布局上存在明确差异。  
> 本项目暂未对"Mono"版本进行实际测试，未确定功能实现绝对成立。  

> [!IMPORTANT]
> **代码重构说明**  
> 当前项目包含'**相当大量**'的AI重构代码，可能存在大量维护性问题。  
> 功能实现集中在 `include/er6`，入口侧尽量保持薄封装。  

## 访问器

- **最小内存访问抽象**：以 `IMemoryAccessor` 为核心，算法层只依赖读内存接口。
- **Windows 适配**：默认提供 WinAPI 实现（`ReadProcessMemory`）适配器。
- **Header-only**：纯头文件库。

## 功能概览

| 功能 | 说明 |
|:-----|:-----|
| **AutoInit + 上下文管理** | 自动发现 Unity 进程、定位 UnityPlayer/GameAssembly、缓存关键 offset/全局槽，并暴露 `g_ctx + Mem()` 读写入口 |
| **跨进程内存访问** | 以 `IMemoryAccessor` 为核心，提供 WinAPI 适配，所有算法都只依赖统一的读内存接口 |
| **GOM 扫描与遍历** | 盲扫 GameObjectManager、校验桶结构、枚举 GameObject/组件，支持按 tag/名称/组件类型快速搜索 |
| **MSID 全局注册表** | 枚举 `UnityEngine.Object` 实例，筛选 GameObject/ScriptableObject，读取名称、InstanceID、托管类型等信息 |
| **Transform / Camera / W2S** | 解析 Transform 层级得到世界坐标；读取相机视图投影矩阵并完成世界坐标到屏幕坐标转换 |
| **Bones** | 遍历 Transform 子树获取骨骼索引、名称与世界坐标 |
| **IL2CPP Metadata + Hint 导出** | 自动扫描 metadata header、导出 `global-metadata.dat`，并可生成包含注册信息的 hint json |
| **DumpSDK6 工具链** | 结合 metadata/hint 结果生成 C# API 描述与泛型结构信息，辅助离线分析/SDK 导出 |
| **模块化 Header-only 设计** | `er6/unity6/*` 下分门别类的子模块（gom/msid/object/camera/transform/metadata 等），可按需引用 |

---

## 核心 API 列表

| 模块 | 代表 API | 说明 |
|:----|:---------|:-----|
| **上下文 / AutoInit** | `AutoInit()` / `ResetContext()` / `IsInited()` | 自动发现 Unity 进程、刷新 `g_ctx`，并提供重置与状态查询 |
|  | `ReadPtr(addr)` / `ReadValue<T>(addr)` | 基于 `g_ctx + Mem()` 的统一读内存封装（支持返回值与 out 版本） |
| **GOM** | `GomManager()` / `GomBucketsPtr()` / `FindGameObjectThroughTag(tag)` | 访问 GameObjectManager、遍历桶并按 tag 搜索 |
|  | `EnumerateGameObjects()` / `GetComponentThroughTypeId(go, typeId)` | 列举 GameObject，或按组件 typeId/typeName 精确取组件 |
| **MSID** | `MsIdSetPtr()` / `MsIdEntriesBase()` / `MsIdCount()` | 读取 ms_id_to_pointer set 元数据 |
|  | `EnumerateMsIdToPointerObjects(opt)` / `FindObjectsOfTypeAll(ns, name)` | 枚举或按命名空间+类型名查找 `UnityEngine.Object` 实例 |
| **对象/名称** | `ReadGameObjectName(nativeGo)` / `ReadScriptableObjectName(nativeSo)` | 读取常见 Native/Managed 对象名称 |
| **Transform / Camera / W2S** | `GetTransformWorldPosition(transformPtr)` | 解析层级状态，输出世界坐标 |
|  | `FindMainCamera()` / `GetCameraMatrix(nativeCamera)` / `W2S(viewProj, screen, world)` | 找主相机、读取视图投影矩阵并执行世界转屏幕 |
| **Bones** | `GetBoneTransformAll(rootGo)` / `GetTransformWorldPosition(transform)` | 遍历 Transform 子树获取骨骼信息与世界坐标 |
| **Metadata / Hint** | `ExportGameAssemblyMetadataByScore()` / `ExportGameAssemblyMetadataHintJsonTScoreToSidecar(path)` | 一次性导出 metadata bytes 与 hint json |
| **DumpSDK6** | `DumpSdk6Dump(paths)` / `DumpSdk6DumpByPid(pid, paths)` | 组合 metadata/hint 结果，生成 C# API 与泛型结构描述（推荐 AutoInit 后使用无 pid 版本） |

---

## 编译要求

- **C++ 标准**：C++17
- **编译器**：MSVC（Visual Studio 2022）
- **平台**：Windows x64
- **链接方式**：静态运行时（/MT）
- **第三方库**：仓库已包含 `glm/`（工具侧编译 bat 已默认添加 include）

---

<details>
<summary><strong>目录结构</strong></summary>

```
ExternalResolve6/
├── Resolve6.hpp                # 统一入口头文件
├── include/
│   └── er6/
│       ├── core/               # 基础类型定义
│       ├── mem/                # 内存访问抽象
│       ├── os/win/             # Windows 平台实现
│       └── unity6/
│           ├── camera/         # 相机与 W2S
│           ├── core/           # 偏移定义
│           ├── dumpsdk/        # SDK 导出
│           ├── gom/            # GameObjectManager
│           ├── init/           # AutoInit 封装层
│           ├── metadata/       # IL2CPP 元数据
│           ├── msid/           # InstanceID 映射
│           ├── object/         # Native/Managed 对象
│           ├── transform/      # Transform 世界坐标
│           └── util/           # 工具函数
├── Analysis/
│   ├── Algorithms/             # 算法说明文档 (18 篇)
│   └── Structures/             # 结构说明文档 (24 篇)
├── tools/                      # 示例工具
└── docs/                       # 补充文档
```

</details>

---

## 快速开始

```cpp
#include "Resolve6.hpp"

int main()
{
    // 自动发现 Unity 进程并初始化上下文
    if (!er6::AutoInit())
    {
        return 1;
    }

    // 查找主相机
    const std::uintptr_t cam = er6::FindMainCamera();
    if (!cam)
    {
        return 1;
    }

    // 获取相机矩阵
    const auto viewProjOpt = er6::GetCameraMatrix(cam);
    if (!viewProjOpt.has_value())
    {
        return 1;
    }

    // 获取骨骼信息示例
    const std::uintptr_t targetGo = er6::FindGameObjectThroughTag(5);
    if (targetGo)
    {
        auto bones = er6::GetBoneTransformAll(targetGo);
        for (const auto& bone : bones)
        {
            auto pos = er6::GetTransformWorldPosition(bone.transform);
            // 使用 bone.index, bone.boneName, pos
        }
    }

    er6::ResetContext();
    return 0;
}
```

---

## AutoInit 与 init/* 封装

- `er6::AutoInit()` 会自动定位 Unity 进程并填充全局上下文 `g_ctx`
- `include/er6/unity6/init/*` 提供基于 `g_ctx + Mem()` 的薄封装，尽量减少手动传参
- 关键函数列表与最小必要参数汇总见：`docs/autoinit_functions.md`
- 算法与结构详细说明见：`Analysis/Algorithms/` 与 `Analysis/Structures/`

---

<div align="center">

**Platform:** Windows x64

</div>
