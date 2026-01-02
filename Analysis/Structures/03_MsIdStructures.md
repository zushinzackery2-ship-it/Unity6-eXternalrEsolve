# MSID 结构 - InstanceID 到指针映射

## 概述

它是 Unity 内部全局注册哈希表(GlobalObjectRegistry)，将 InstanceID 映射到 Native 对象指针。
它所保存内容远比GameObjectManager更加全面，它保存的是当前进程内存中所有活跃的 `UnityEngine.Object` 及其子类的实例指针，包括核心游戏对象与组件、资源对象、物理与碰撞、UI/2D、脚本对象等所有被Object类赋予Id属性的对象。

## 原始结构

```
#pragma pack(push, 1)
struct MsIdToPointerSetRaw    // 大小 = 0x10
{
    entriesBase  : uintptr_t   // +0x00  条目数组指针
    capacity     : uint32_t    // +0x08  桶数量(capacity)
    count        : uint32_t    // +0x0C  有效元素数量(count)
}

struct MsIdToPointerEntryRaw  // 大小 = 0x18
{
    hashMask : uint32_t   // +0x00  hashMask
    unk04    : uint32_t   // +0x04  未知
    key      : uint32_t   // +0x08  实例 ID
    unk0C    : uint32_t   // +0x0C  未知
    object  : uintptr_t   // +0x10  Native 对象指针
}
#pragma pack(pop)
```

## 封装结构

```
struct MsIdToPointerSet
{
    set : uintptr_t   // 指向 MsIdToPointerSetRaw 的指针
}

struct ObjectInfo
{
    native       : uintptr_t   // Native 对象地址
    instanceId   : uint32_t    // Unity 实例 ID
    objectName   : string      // m_Name 字段值
    typeFullName : string      // "命名空间.类名"
    kind         : ObjectKind  // 当前枚举实现仅区分 GameObject/ScriptableObject
}

enum class ObjectKind : uint8_t
{
    GameObject = 1,
    ScriptableObject = 2
}

struct EnumerateOptions
{
    onlyGameObject       : bool     // 过滤 GameObject
    onlyScriptableObject : bool     // 过滤 ScriptableObject
    filterLower          : string   // 小写子串过滤
}

 备注：MSID 表本身并不只包含 GameObject/ScriptableObject；
 当前枚举实现会主动跳过其它 `UnityEngine.Object` 子类。
```

## 内存布局图

### 全局槽位置
```
UnityPlayer 模块
    .data / .rdata 段
        |
        +offset --> MsIdToPointerSlot (指向 Set 的指针)
                    |
                    v
                    MsIdToPointerSetRaw
```

### MsIdToPointerSet
```
MsIdToPointerSetRaw
+0x00  entriesBase -----> Entry[0]
+0x08  capacity          Entry[1]
+0x0C  count             Entry[2]
                          ...
                         Entry[capacity-1]
```

### Entry 数组
```
Entry[i] (stride = 0x18)
+0x00  hashMask
+0x04  unk04
+0x08  key (instanceId)
+0x0C  unk0C
+0x10  object -------> Native Unity 对象
                       +0x00 vtable
                       +0x08 instanceId
                       +0x18 gchandle
                       ...
```

## 特殊键值

- `key == 0` : 空槽
- `key == 0xFFFFFFFF` : 已删除/无效
- `key == 0xFFFFFFFE` : 保留
