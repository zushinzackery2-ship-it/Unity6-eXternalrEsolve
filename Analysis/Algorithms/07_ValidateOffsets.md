# Offsets 校验算法

## 目的

通过“读真实数据 + 合理性检查”，快速判断 `TransformOffsets` / `CameraOffsets` 是否正确。

## 变换偏移校验（ValidateTransformOffsets）

输入：
- `mem`
- `TransformOffsets off`
- `transformAddress`
- `maxDepth`

流程：
- 调用 `GetTransformWorldPosition(mem, off, transformAddress, pos, maxDepth)`
- 判断 `pos.x/y/z` 是否都是有限数（`std::isfinite`）

输出：
- world position 能成功计算且是有限数 => true

## 相机偏移校验（ValidateCameraOffsets）

输入：
- `mem`
- `CameraOffsets off`
- `nativeCamera`

流程：
- 调用 `GetCameraViewProjMatrix(mem, nativeCamera, off, m[16])`
- 检查 16 个 float 是否都是有限数
- 累加 `sumAbs += abs(m[i])`
- `sumAbs > 0.0001` 认为矩阵非全零、非异常

输出：
- 读取成功 + 数值正常 => true
