# Native Component 结构

## 概述

NativeComponent 是 Unity 组件的原生层表示，包含托管对象引用、所属 GameObject 和启用状态。

## 组件操作

### GetNativeComponentManaged

```
输入:
  - nativeComponent: Native 组件指针
  - off: GomOffsets

输出:
  - outManaged: 托管组件指针

读取:
  nativeComponent + off.component.managed -> outManaged
```

### GetNativeComponentGameObject

```
输入:
  - nativeComponent: Native 组件指针
  - off: GomOffsets

输出:
  - outGameObject: 所属 Native GameObject 指针

读取:
  nativeComponent + off.component.game_object -> outGameObject
```

### IsComponentEnabled

```
输入:
  - nativeComponent: Native 组件指针
  - off: GomOffsets

输出:
  - bool: 组件是否启用

读取:
  nativeComponent + off.component.enabled -> byte
  return byte != 0
```

## 内存布局

```
NativeComponent
+0x00  vtable
+0x18  managed -------> 托管 Component
+0x30  game_object --> 所属 NativeGameObject
+0x38  enabled (byte)
```

## 偏移定义

```
struct NativeComponentOffsets
{
    managed     = 0x18   // -> 托管对象指针
    game_object = 0x30   // -> 所属 GameObject 指针
    enabled     = 0x38   // -> 启用状态 (byte)
}
```

## 组件链路

```
NativeComponent
    |
    +0x18 managed --> ManagedComponent
    |                     |
    |                     +-> Il2CppClass (类型信息)
    |
    +0x30 game_object --> NativeGameObject
                              |
                              +0x50 name_ptr --> "ObjectName"
```
