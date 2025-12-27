<div align="center">

# ExternalResolve6

**Unity6 运行时内存结构算法还原库（er6 / header-only）**

*跨进程读取 | Header-Only | IL2CPP*

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
> 当前项目包含**相当大量**的AI重构代码，可能存在大量屎山。
> 功能实现集中在 `include/er6`，入口侧尽量保持薄封装。

## 访问器

- **最小内存访问抽象**：以 `IMemoryAccessor` 为核心，算法层只依赖读内存接口。
- **Windows 适配**：默认提供 WinAPI 实现（`ReadProcessMemory`）适配器。
- **Header-only**：纯头文件库。

## 特性

| 功能 | 说明 |
|:-----|:-----|
| **进程/模块解析** | Windows 下枚举进程、查询模块基址与大小 |
| **跨进程内存访问** | 通过 `IMemoryAccessor` 读取目标进程内存 |
| **GameObject/组件读取** | 读取 GOM/MSID 相关结构并枚举对象 |
| **Transform 世界坐标** | 读取层级并计算世界坐标 |
| **相机 W2S** | 读取相机矩阵，世界坐标转屏幕坐标 |
| **IL2CPP Metadata 扫描/导出** | 扫描并导出 `global-metadata.dat`，可输出 sidecar hint json |
| **DumpSDK6 导出** | 导出 `dump.cs` 与 `generic.json` |

---

## 编译要求

- **C++ 标准**：C++17
- **编译器**：MSVC（Visual Studio 2022）
- **平台**：Windows x64
- **链接方式**：静态运行时（/MT）
- **第三方库**：需自行下载并在项目中添加 **glm**

---

<details>
<summary><strong>目录结构</strong></summary>

```
ExternalResolve6/
├── Resolve6.hpp
├── include/
│   └── er6/
│       ├── core/
│       ├── mem/
│       ├── os/
│       │   └── win/
│       └── unity6/
│           ├── camera/
│           ├── core/
│           ├── dumpsdk/
│           ├── gom/
│           ├── init/
│           ├── metadata/
│           ├── msid/
│           ├── object/
│           ├── transform/
│           └── util/
└── Analysis/
    ├── Algorithms/
    └── Structures/
```

</details>

---

## 快速开始

```cpp
#include "Resolve6.hpp"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    if (!er6::AutoInit())
    {
        return 1;
    }

    const std::uintptr_t cam = er6::FindMainCamera();
    if (!cam)
    {
        return 1;
    }

    const auto viewProjOpt = er6::GetCameraMatrix(cam);
    if (!viewProjOpt.has_value())
    {
        return 1;
    }

    er6::ResetContext();
    return 0;
}
```

---

## AutoInit 与 init/* 封装

- `er6::AutoInit()` 会自动定位 Unity 进程并填充全局上下文 `g_ctx`。
- `include/er6/unity6/init/*` 提供基于 `g_ctx + Mem()` 的薄封装，尽量减少手动传参。
- 关键函数列表与最小必要参数汇总见：`docs/autoinit_functions.md`

---

## Smoke Tests

- `tools/smoke_test6/`：基于 AutoInit 的综合验证（MSID/GOM/Camera/W2S/Metadata）。

---

<div align="center">

**Platform:** Windows x64

</div>
