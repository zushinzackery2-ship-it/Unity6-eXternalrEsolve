# ManagedBackend / ManagedObject 结构

## ManagedBackend 功能标记

### ManagedBackend

```
enum class ManagedBackend : std::uint8_t
{
    Mono = 1,
    Il2Cpp = 2,
};
```

### Feature

```
enum class Feature : std::uint8_t
{
    MsIdToPointer = 1,
    ManagedObjectChain = 2,
    ManagedClassName = 3,
    TypeManager = 4,
    Il2CppMetadata = 5,
};
```

### Supports

规则：
- `Feature::Il2CppMetadata` 仅在 `backend == Il2Cpp` 时为真
- 其它 feature 默认都返回 true

## ManagedObject 类型信息

### TypeInfo

```
struct TypeInfo
{
    std::string name;
    std::string namespaze;
};
```

### ReadManagedObjectKlass

链路：
- `ManagedObject + off.managed_cached_gchandle -> gchandle`
- `gchandle + off.gchandle_to_klass -> klass`

### ReadManagedObjectTypeInfo

流程：
- 先 `ReadManagedObjectKlass` 得到 `klass`
- 再 `ReadIl2CppClassName` 读取 `namespace` 与 `className`
- 输出到 `TypeInfo{name, namespaze}`

### ReadManagedObjectClassFullName

拼接规则：
- `namespaze` 为空：`fullName = name`
- 否则：`fullName = namespaze + "." + name`
