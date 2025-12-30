#pragma once

// 统一入口：包含 init.hpp 即可获得所有 AutoInit 封装 API
// init.hpp 已包含：context, gom, component, camera, w2s, transform, object, msid, metadata, dumpsdk, bones
#include "er6/unity6/init.hpp"

// metadata 导出（init/metadata.hpp 只包含部分，这里补全完整 metadata 功能）
#include "er6/unity6/metadata.hpp"
