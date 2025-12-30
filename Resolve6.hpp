#pragma once

// 统一入口：包含 init.hpp 即可获得所有 AutoInit 封装 API
#include "er6/unity6/init.hpp"

// metadata 导出（init/metadata.hpp 只包含部分，这里补全）
#include "er6/unity6/metadata.hpp"

// 底层模块（如需直接使用底层 API）
#include "er6/unity6/gom/gom_walker.hpp"
#include "er6/unity6/gom/gom_search.hpp"
#include "er6/unity6/msid/enumerate_objects.hpp"
#include "er6/unity6/msid/find_objects_of_type_all.hpp"
#include "er6/unity6/object/managed/managed_object.hpp"
#include "er6/unity6/object/native/native_game_object.hpp"
#include "er6/unity6/camera/world_to_screen.hpp"
