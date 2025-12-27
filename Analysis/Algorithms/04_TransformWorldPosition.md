# Transform 世界坐标算法

## 目的

通过遍历父级层级并累积局部变换来计算 Transform 的世界坐标。

## 算法

### GetTransformWorldPosition

```
输入:
  - mem: IMemoryAccessor
  - off: TransformOffsets
  - transformAddress: Native Transform 指针
  - maxDepth: 最大层级深度

输出:
  - outPos: glm::vec3 世界坐标
  - bool 成功

步骤:
1. 从 transform 读取 TransformHierarchyState
2. 调用 ComputeWorldPositionFromHierarchy
```

### ReadTransformHierarchyState

```
链:
  transform+0x28 --> statePtr
  statePtr+0x18 --> nodeData
  statePtr+0x20 --> parentIndices
  transform+0x30 --> index
```

### ComputeWorldPositionFromHierarchy

```
算法 (SIMD 优化):
1. 读取 nodeData + index * 48 处的自身节点
2. acc = selfNode.position (SSE __m128)
3. 从 parentIndices[index] 读取父索引
4. 当 parent >= 0 且 depth < maxDepth:
   a. 检查循环 (visited 集合)
   b. 读取父节点
   c. 提取: t (位置), q (旋转), m (缩放)
   d. 应用变换: acc = t + rotate(scale(acc, m), q)
   e. 读取下一个父索引
   f. depth++
5. 将 acc 存储到 outPos (x, y, z)
```

## 节点数据布局（48 字节）

```
float[12]:
  [0-3]   position (x, y, z, w)
  [4-7]   rotation 四元数 (x, y, z, w)
  [8-11]  scale (x, y, z, w)
```

## 循环检测

使用 `unordered_set<int32>` 检测畸形层级中的无限循环。
