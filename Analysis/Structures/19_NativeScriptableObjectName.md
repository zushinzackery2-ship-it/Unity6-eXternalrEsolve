# Native ScriptableObject 名称读取

## 概述

读取 ScriptableObject 的 m_Name 字段，与 GameObject 名称读取类似但使用不同偏移。

## 读取操作

### ReadScriptableObjectName

```
输入:
  - mem: IMemoryAccessor
  - scriptableObject: Native ScriptableObject 指针
  - off: Offsets

输出:
  - out: string (对象名称)

步骤:
1. 读取 scriptableObject + off.scriptable_object_name_ptr -> namePtr
2. 验证 namePtr 是否为有效用户态指针
3. 从 namePtr 读取 C 字符串 (最大 256 字符)
```

## 内存布局

```
NativeScriptableObject
+0x00  vtable
+0x08  instanceId
+0x18  managed
...
+off.scriptable_object_name_ptr --> namePtr --> "ScriptableObjectName\0"
```

## 与 GameObject 名称的区别

| 对象类型 | 偏移字段 | 说明 |
|---------|---------|------|
| GameObject | off.game_object.name_ptr | 在 GomOffsets 中定义 |
| ScriptableObject | off.scriptable_object_name_ptr | 在 Offsets 中定义 |

## 用途

- MSID 枚举时区分 GameObject 和 ScriptableObject
- 按名称搜索 ScriptableObject
- 调试和日志输出
