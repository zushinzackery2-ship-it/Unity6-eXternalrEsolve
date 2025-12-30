# 世界转屏幕算法

## 目的

使用相机的视图投影矩阵将 3D 世界坐标投影到 2D 屏幕坐标。

## 算法

### WorldToScreenPoint

```
输入:
  - viewProj: glm::mat4 (4x4 视图投影矩阵)
  - screen: ScreenRect { x, y, width, height }
  - worldPos: glm::vec3

输出:
  - WorldToScreenResult { visible, x, y, depth }

步骤:
1. clip = viewProj * vec4(worldPos, 1.0)
2. 如果 clip.w <= 0.001: 返回不可见
3. ndc = clip.xyz / clip.w
4. sx = (ndc.x + 1.0) * 0.5 * width + screen.x
5. sy = (1.0 - ndc.y) * 0.5 * height + screen.y
6. visible = ndc 在 [-1,1] 范围内
7. 返回 { visible, sx, sy, clip.w }
```

## 坐标系统

```
世界空间: Y 上, Z 前, X 右
NDC: [-1,1] x [-1,1]
屏幕空间: (0,0) 左上角, Y 向下
```

## 相机 0xF0 偏移矩阵

组合后的 View * Projection 矩阵。
