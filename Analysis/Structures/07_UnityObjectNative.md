# Unity Native Object 基础结构

## UnityPlayer 地址范围（UnityPlayerRange）

用于判断一个指针是否落在 UnityPlayer 模块范围内。

```
struct UnityPlayerRange
{
    std::uintptr_t base = 0;
    std::uint32_t size = 0;

    bool Contains(std::uintptr_t p) const
    {
        if (!base || size == 0)
        {
            return false;
        }
        return p >= base && p < base + static_cast<std::uintptr_t>(size);
    }
};
```

## 原生 Unity 对象判定链

### IsProbablyUnityObject

用于快速筛掉无效指针：

- **Canonical**：`IsCanonicalUserPtr(obj)` 必须为真
- **对齐**：`(obj & 0x7) == 0`
- **vtable 归属**：读取 `obj+0x00` 的 `vtable`，要求 `UnityPlayerRange.Contains(vtable)`
- **managed 指针有效**：读取 `obj + off.unity_object_managed_ptr`，要求 canonical

## MSID（ms_IDToPointer）与原生 Unity 对象

MSID（`Object::ms_IDToPointer`）是 `UnityEngine.Object` 的全局注册哈希表（InstanceID -> Native Object*）。
因此从 MSID 条目里拿到的 `object` 指针，理论上都应当满足本文件的 `IsProbablyUnityObject` 判定链。

## 常用读取

### ReadUnityObjectInstanceId

读取 `obj + off.unity_object_instance_id` 的实例 ID（`uint32`）。

### ReadUnityObjectKlass

按链路取出 `Il2CppClass*`：

- `NativeObject + off.unity_object_managed_ptr -> managed`
- `managed + off.managed_cached_gchandle -> gchandle`
- `gchandle + off.gchandle_to_klass -> klass`

（说明中我统一用 `Base+0x??->Field` 这种形式，避免 `*()` 解引用写法）
