#pragma once

#include <cstdint>

namespace er6
{

struct GomManagerOffsets
{
    std::uint32_t buckets_ptr = 0x00;
    std::uint32_t bucket_count = 0x08;
    std::uint32_t local_game_object_list_head = 0x18;
};

struct GomBucketOffsets
{
    std::uint32_t hash_mask = 0x00;
    std::uint32_t key = 0x08;
    std::uint32_t flags = 0x0C;
    std::uint32_t value = 0x10;
    std::uint32_t stride = 24;
};

struct GomListNodeOffsets
{
    std::uint32_t next = 0x08;
    std::uint32_t native_object = 0x10;
};

struct NativeGameObjectOffsets
{
    std::uint32_t managed = 0x18;
    std::uint32_t component_pool = 0x20;
    std::uint32_t component_count = 0x30;
    std::uint32_t tag_raw = 0x44;
    std::uint32_t name_ptr = 0x50;
};

struct NativeComponentOffsets
{
    std::uint32_t managed = 0x18;
    std::uint32_t game_object = 0x30;
    std::uint32_t enabled = 0x38;
};

struct ComponentPoolOffsets
{
    std::uint32_t slot_stride = 0x10;
    std::uint32_t slot_type_id = 0x00;
    std::uint32_t slot_native = 0x08;
};

struct GomOffsets
{
    GomManagerOffsets manager;
    GomBucketOffsets bucket;
    GomListNodeOffsets node;
    NativeGameObjectOffsets game_object;
    NativeComponentOffsets component;
    ComponentPoolOffsets pool;
};

} // namespace er6
