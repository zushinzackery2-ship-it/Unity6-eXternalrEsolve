# FindMainCamera 算法

## 目的

通过 GOM 查找主相机（MainCamera）的 Native Camera 组件。

## 算法

### FindMainCamera (底层)

```
输入:
  - mem: IMemoryAccessor
  - gomGlobalSlot: GOM 全局槽地址
  - gomOff: GomOffsets
  - mainCameraTag: 主相机 tag (默认 5)
  - cameraTypeId: Camera 组件类型 ID

输出:
  - outNativeCamera: Native Camera 组件指针

步骤:
1. mainGo = FindGameObjectThroughTag(mainCameraTag)
2. 如果 mainGo 为空则失败
3. camNative = GetComponentThroughTypeId(mainGo, cameraTypeId)
4. 如果 camNative 为空则失败
5. 检查 IsComponentEnabled(camNative)
6. 返回 camNative
```

### FindMainCamera (init 层封装)

```
输入: 无 (使用 g_ctx)

输出:
  - nativeCamera: uintptr_t

步骤:
1. mainCameraTag = 5
2. mainGo = FindGameObjectThroughTag(mainCameraTag)
3. 如果 mainGo 为空则返回 0
4. nativeCamera = GetCameraComponent(mainGo)
5. 返回 nativeCamera
```

## 关键常量

```
MainCamera Tag = 5  (Unity 内置 tag)
Camera ViewProjMatrix 偏移 = 0xF0
```

## 相机矩阵读取

### GetCameraViewProjMatrix

```
输入:
  - nativeCamera: Native Camera 指针
  - off: CameraOffsets

输出:
  - outMatrix: float[16]

读取:
  nativeCamera + off.view_proj_matrix (0xF0) -> 64 字节
```

### GetCameraMatrix

```
输入:
  - nativeCamera: Native Camera 指针
  - off: CameraOffsets

输出:
  - outViewProj: glm::mat4

步骤:
1. 读取 float[16] 原始数据
2. 转换为 glm::mat4
```

## 对外 API（init 层）

```cpp
// 查找主相机
FindMainCamera() -> uintptr_t

// 获取相机矩阵
GetCameraMatrix(nativeCamera) -> optional<glm::mat4>
```

## 用途

- 获取主相机用于 World-to-Screen 转换
- 读取视图投影矩阵进行坐标变换
