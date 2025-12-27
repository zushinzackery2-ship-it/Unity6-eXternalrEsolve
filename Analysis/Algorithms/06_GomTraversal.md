# GOM 遍历算法

## 目的

按 tag 查找 GameObject 并遍历组件池。

## 通过标签查找游戏对象（GameObject）

```
步骤:
1. 从 GOM 槽读取 manager
2. 读取 buckets 指针和数量
3. 对于每个 bucket:
   - 如果 key == tag:
     - value+0x00 --> listHead
     - listHead+0x10 --> native GameObject
     - 返回
```

## 通过类型 ID 获取组件

```
步骤:
1. 从 gameObject+0x20 读取 pool
2. 从 gameObject+0x30 读取 count
3. 对于每个 slot (stride 0x10):
   - 如果 typeId 匹配: 返回 slot+0x08
```

## 列表遍历

```
node = listHead
while node:
  native = node+0x10
  next = node+0x08
  if next == listHead: break
  node = next
```

## 已知标签

| Tag | 名称 |
|-----|------|
| 5 | MainCamera |
| 6 | Player |
