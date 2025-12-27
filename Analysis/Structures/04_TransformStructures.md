# Transform 结构 - 层级与位置数据

## 概述

Unity Transform 使用层级状态结构来高效计算世界坐标。
Transform 数据存储在连续数组中，通过 transform index 索引。

## 变换偏移（TransformOffsets）结构

```
struct TransformOffsets
{
    state_ptr           = 0x28   // Transform -> state 指针
    index               = 0x30   // Transform -> 层级中的索引

    state_node_data     = 0x18   // State -> 节点数据数组
    state_parent_indices = 0x20  // State -> 父索引数组

    node_stride         = 48     // 节点大小 (0x30)
}
```

## 变换层级状态（TransformHierarchyState）结构

```
struct TransformHierarchyState
{
    nodeData      : uintptr_t   // 节点数组指针
    parentIndices : uintptr_t   // 父索引数组指针
}
```

## 内存布局图

### Transform 对象
```
NativeTransform
+0x28  state_ptr -------> TransformState
+0x30  index (int32)
```

### TransformState
```
TransformState
+0x18  nodeData --------> Node[0], Node[1], ...
+0x20  parentIndices ---> int32[0], int32[1], ...
```

### 节点数据 (stride = 48 字节)
```
Node[i]
+0x00  position.x (float)
+0x04  position.y (float)
+0x08  position.z (float)
+0x0C  position.w (float)    // 通常为 0 或 1
+0x10  rotation.x (float)    // 四元数
+0x14  rotation.y (float)
+0x18  rotation.z (float)
+0x1C  rotation.w (float)
+0x20  scale.x (float)
+0x24  scale.y (float)
+0x28  scale.z (float)
+0x2C  scale.w (float)       // 通常为 1
```

### 父索引数组
```
parentIndices[i] = 父 transform 索引
                 = -1 表示根节点 (无父节点)
```

## 层级示例

```
Root (index=0, parent=-1)
  +-- Child1 (index=1, parent=0)
  |     +-- GrandChild (index=2, parent=1)
  +-- Child2 (index=3, parent=0)

parentIndices = [-1, 0, 1, 0]
```
