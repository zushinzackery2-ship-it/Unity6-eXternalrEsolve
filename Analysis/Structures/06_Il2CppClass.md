# Il2CppClass 结构 - 类型信息

## 概述

Il2CppClass 是 IL2CPP 编译的 Unity 游戏的运行时类型信息结构。
包含类名、命名空间和继承链。

## 布局（来自 Offsets）

```
Il2CppClass
+0x10  name      : char*           // 类名指针
+0x18  namespace : char*           // 命名空间字符串指针
+0x58  parent    : Il2CppClass*    // 父类指针
```

## 内存布局图

### 类型链示例

```
UnityEngine.MonoBehaviour
    |
    +0x58 parent
    |
    v
UnityEngine.Behaviour
    |
    +0x58 parent
    |
    v
UnityEngine.Component
    |
    +0x58 parent
    |
    v
UnityEngine.Object
    |
    +0x58 parent
    |
    v
System.Object
    |
    +0x58 parent = NULL (根)
```

### 字符串格式

```
Il2CppClass
+0x10 --> "MonoBehaviour\0"
+0x18 --> "UnityEngine\0"
```

完整类型名: `namespace.name` = `UnityEngine.MonoBehaviour`

## 有效标识符规则

- 名称必须以 A-Z、a-z 或 _ 开头
- 主体可包含: A-Z、a-z、0-9、_、`、+
- 命名空间可额外包含: .
- 最大长度: 128 字符
- 仅 ASCII 可打印字符 (0x20-0x7E)
