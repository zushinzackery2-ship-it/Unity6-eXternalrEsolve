# Native GameObject / Component / Transform 结构

## 原生 GameObject（NativeGameObject）

提供对 GameObject 常用字段的读取封装：

- `GetNativeGameObjectManaged(GameObject + off.game_object.managed -> managed)`
- `GetNativeGameObjectTag(GameObject + off.game_object.tag_raw -> raw)`
  - `tag = (raw & 0xFFFF)`
- `ReadNativeGameObjectName(GameObject + off.game_object.name_ptr -> namePtr -> cstring)`

组件相关：

- `GetNativeGameObjectComponentTypeIds`：
  - 通过 `GetComponentPool` + `GetComponentCount` 遍历 slot，收集 `typeId`
- `GetNativeGameObjectComponent(index)`：
  - 通过 slot 直接取 `nativeComponent`

## 原生组件（NativeComponent）

- `GetNativeComponentManaged(Component + off.component.managed -> managed)`
- `GetNativeComponentGameObject(Component + off.component.game_object -> gameObject)`
- `IsComponentEnabled(Component + off.component.enabled -> bool)`

## 原生 Transform（NativeTransform）

Transform 在这里视为一种 Component：

- 通过 **typeId** 或 **typeName** 在 GameObject 组件池中定位 Transform：
  - `FindTransformOnGameObjectThroughTypeId`
  - `FindTransformOnGameObjectThroughTypeName`
  - `FindTransformOnGameObject` 默认用 typeName="Transform"

世界坐标：

- `GetGameObjectWorldPositionThroughTransformTypeId`
  - `GameObject -> TransformComponent -> GetTransformWorldPosition`
- `GetGameObjectWorldPositionThroughTransformTypeName`

## 名称读取

- `ReadGameObjectName(GameObject + off.game_object_name_ptr -> namePtr -> cstring)`
- `ReadScriptableObjectName(ScriptableObject + off.scriptable_object_name_ptr -> namePtr -> cstring)`

字符串读取上限：256 字符。
