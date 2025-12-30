# 初始化上下文与封装调用

## 上下文结构（Context）

`Context` 保存：
- 进程信息：`pid`、`process(HANDLE)`
- 运行时类型：`ManagedBackend runtime`
- UnityPlayer 模块信息与范围：`unityPlayer`、`unityPlayerRange`
- 关键 offsets：`Offsets off`、`GomOffsets gomOff`、`CameraOffsets camOff`、`TransformOffsets transformOff`
- 关键全局槽：`gomGlobalSlotVa/Rva`、`msIdToPointerSlotVa/Rva`

全局实例：`g_ctx`。

## 自动初始化（AutoInit）流程

1. `FindUnityWndClassPids()` 找到候选 pid 列表
2. 对每个 pid：
   - `GetRemoteModuleInfo` 获取 UnityPlayer 模块信息
   - `GetRemoteModuleInfo` 尝试获取 GameAssembly 模块信息，成功则 runtime=Il2Cpp，否则 Mono
   - `InitSettings(pid, runtime)`：打开进程句柄、获取 UnityPlayer 模块信息
   - `FindGomGlobalSlotRvaByScan` 扫描得到 GOM slot RVA
   - `FindMsIdToPointerSlotVaByScan` 扫描得到 MSID slot VA
   - 写回 `g_ctx` 并返回 true
3. 全失败则 `ResetContext()` 并返回 false

## 封装层（基于全局上下文的薄封装）

### 基础上下文与读内存

通过 `g_ctx` + `Mem()` 提供基础 API：
- `AutoInit()` / `ResetContext()` / `IsInited()`
- `Pid()` / `UnityPlayerBase()` / `GomGlobalSlotVa()` / `MsIdToPointerSlotVa()`
- 读内存封装：
  - `ReadPtr(addr, out)` / `ReadPtr(addr)`
  - `ReadValue<T>(addr, out)` / `ReadValue<T>(addr)`

### GOM 封装

通过 `g_ctx` + `Mem()` 提供：
- `GomManager()` / `GomBucketsPtr()` / `GomBucketCount()` / `GomLocalGameObjectListHead()`
- `FindGameObjectThroughTag(tag)`
- `EnumerateGameObjects(out)` / `EnumerateGameObjects()`
- `GetListNodeNative(node, out)` / `GetListNodeNative(node)`
- `GetListNodeNext(node, out)` / `GetListNodeNext(node)`

### MSID 封装

- `MsIdSetPtr()`：读取 set 指针
- `MsIdEntriesBase()` / `MsIdCount()`：读取 entriesBase 与 count
- `EnumerateMsIdToPointerObjects(opt, cb)`：包装 enumerate（当前实现仅枚举 GameObject/ScriptableObject；底层 MSID 表是 `UnityEngine.Object` 全局注册表）
- `EnumerateMsIdToPointerObjects(opt)`：直接返回数组（收集 ObjectInfo）
- `FindObjectsOfTypeAll(className)`：1 参数默认 className
- `FindObjectsOfTypeAll(namespaceOpt, className)`：2 参数默认第一个是 namespace

### 对象/类型信息读取封装

- `ReadGameObjectName(obj, outName)` / `ReadGameObjectName(obj)`
- `ReadScriptableObjectName(obj, outName)` / `ReadScriptableObjectName(obj)`
- `ReadNativeGameObjectName(obj, outName)` / `ReadNativeGameObjectName(obj)`
- `ReadManagedObjectTypeInfo(managed, out)` / `ReadManagedObjectTypeInfo(managed)`

### 组件封装

- `GetComponentThroughTypeId(gameObjectNative, typeId)`
- `GetComponentThroughTypeName(gameObjectNative, typeName)`
- `GetTransformComponent(gameObjectNative)`

### 相机封装

- `FindMainCameraNative(mainCameraTag, cameraTypeId)`
- `FindMainCamera()`
- `GetCameraMatrix(nativeCamera, outViewProj)` / `GetCameraMatrix(nativeCamera)`

### Transform 封装

- `GetTransformWorldPosition(transformAddress, outPos)` / `GetTransformWorldPosition(transformAddress)`

### W2S（世界坐标到屏幕坐标）

- `W2S(viewProj, screen, world)` 直接调用 `WorldToScreenPoint`

### Metadata 封装

- `TryGetGameAssemblyModuleInfo(out)` / `TryGetGameAssemblyModuleInfo()`
- `ExportGameAssemblyMetadataByScore(outBytes)` / `ExportGameAssemblyMetadataByScore()`
- `ExportGameAssemblyMetadataHintJsonTScoreToSidecar(path)`
