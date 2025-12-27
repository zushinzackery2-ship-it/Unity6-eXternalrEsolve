# Offsets - 核心偏移量结构

## 偏移结构（Offsets）

Unity 对象内存布局的基础偏移配置。

```
struct Offsets
{
    ms_id_to_pointer_rva      = 0x00   // UnityPlayer 模块中 ms_id_to_pointer（MSID，全局 UnityEngine.Object 注册表入口）全局变量的 RVA

    // Native 对象布局
    game_object_name_ptr      = 0x50   // GameObject -> 名称指针
    scriptable_object_name_ptr = 0x38  // ScriptableObject -> 名称指针
    unity_object_instance_id  = 0x08   // Native 对象 -> 实例 ID
    unity_object_managed_ptr  = 0x18   // Native 对象 -> 托管对象指针

    // 托管对象布局
    managed_cached_gchandle   = 0x00   // 托管对象 -> GCHandle
    gchandle_to_klass         = 0x00   // GCHandle -> Il2CppClass*

    // Il2CppClass 布局
    il2cppclass_name_ptr      = 0x10   // Il2CppClass -> 类名字符串指针
    il2cppclass_namespace_ptr = 0x18   // Il2CppClass -> 命名空间字符串指针
    il2cppclass_parent        = 0x58   // Il2CppClass -> 父类指针

    // MSID Set 布局
    ms_id_set_entries_base    = 0x00   // MsIdToPointerSet -> 条目数组基址
    ms_id_set_count           = 0x08   // MsIdToPointerSet -> 条目数量

    // MSID Entry 布局
    ms_id_entry_key           = 0x00   // Entry -> 实例 ID 键
    ms_id_entry_object        = 0x10   // Entry -> Native 对象指针
    ms_id_entry_stride        = 0x18   // Entry 大小 (24 字节)
}
```

## 内存布局图

### Native Unity 对象 (GameObject/ScriptableObject)
```
+0x00  vtable (指向 UnityPlayer 模块内部)
+0x08  instanceId (uint32)
+0x18  managed (托管对象指针)
...
+0x38  [ScriptableObject] 名称指针
...
+0x50  [GameObject] 名称指针
```

### 托管对象
```
+0x00  gchandle (指针)
       |
       v
       +0x00  klass (Il2CppClass*)
```

### Il2CppClass
```
+0x10  name (char*)
+0x18  namespace (char*)
+0x58  parent (Il2CppClass*)
```
