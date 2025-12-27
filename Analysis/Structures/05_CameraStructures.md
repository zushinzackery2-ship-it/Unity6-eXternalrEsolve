# Camera 结构 - 视图投影数据

## 相机偏移结构（CameraOffsets）

```
struct CameraOffsets
{
    view_proj_matrix = 0xF0   // Camera -> 4x4 视图投影矩阵
}
```

## 屏幕矩形（ScreenRect）

```
struct ScreenRect
{
    x      : float   // 屏幕原点 X
    y      : float   // 屏幕原点 Y
    width  : float   // 屏幕宽度 (像素)
    height : float   // 屏幕高度 (像素)
}
```

## 世界到屏幕结果（WorldToScreenResult）

```
struct WorldToScreenResult
{
    visible : bool    // 点是否在屏幕内
    x       : float   // 屏幕 X 坐标
    y       : float   // 屏幕 Y 坐标
    depth   : float   // 距相机距离 (clip.w)
}
```

## 内存布局图

### NativeCamera
```
NativeCamera (Component)
+0x00  vtable
+0x18  managed
+0x30  gameObject
+0x38  enabled
...
+0xF0  viewProjMatrix[16]   // 4x4 float 矩阵 (64 字节)
       [0]  [1]  [2]  [3]   // 第 0 行
       [4]  [5]  [6]  [7]   // 第 1 行
       [8]  [9]  [10] [11]  // 第 2 行
       [12] [13] [14] [15]  // 第 3 行
```

## 视图投影矩阵

偏移 0xF0 处的矩阵是组合后的视图投影矩阵：
```
ViewProj = Projection * View
```

该矩阵将世界坐标直接转换到裁剪空间：
```
clipPos = ViewProj * vec4(worldPos, 1.0)
```

## 主相机（MainCamera）标签

Unity 使用 tag 值 `5` 表示 MainCamera。
