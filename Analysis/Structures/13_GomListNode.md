# GOM ListNode 结构 - 链表节点

## 概述

ListNode 是 GameObjectManager 中用于组织 GameObject 的双向链表节点。
每个 bucket 维护一个 ListNode 链表，存储具有相同 tag 的所有 GameObject。

## 节点操作

### GetListNodeFirst

```
输入:
  - listHead: 链表头指针

输出:
  - outNode: 第一个节点指针

说明:
  listHead 本身就是第一个节点，直接返回。
```

### GetListNodeNext

```
输入:
  - node: 当前节点指针
  - off: GomOffsets

输出:
  - outNext: 下一个节点指针

读取:
  node + off.node.next -> outNext
```

### GetListNodeNative

```
输入:
  - node: 节点指针
  - off: GomOffsets

输出:
  - outNative: Native GameObject 指针

读取:
  node + off.node.native_object -> outNative
```

## 内存布局

```
ListNode (stride 未固定，通过偏移访问)
+0x00  (保留/prev)
+0x08  next -----------> 下一个 ListNode (或 listHead 表示结束)
+0x10  native_object --> NativeGameObject
```

## 遍历模式

```
node = listHead
visited = {}
while node != 0 and node not in visited:
    visited.add(node)
    native = node + off.node.native_object
    // 处理 native
    next = node + off.node.next
    if next == listHead:
        break
    node = next
```

## 循环检测

使用 visited 集合防止无限循环，当 next 等于 listHead 时表示遍历完成。
