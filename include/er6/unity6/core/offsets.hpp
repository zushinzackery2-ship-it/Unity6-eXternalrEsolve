#pragma once

#include <cstdint>

namespace er6
{

struct Offsets
{
    std::uint32_t ms_id_to_pointer_rva = 0;

    std::uint32_t game_object_name_ptr = 0x50;
    std::uint32_t scriptable_object_name_ptr = 0x38;
    std::uint32_t unity_object_instance_id = 0x08;
    std::uint32_t unity_object_managed_ptr = 0x18;

    std::uint32_t managed_cached_gchandle = 0x00;
    std::uint32_t gchandle_to_klass = 0x00;

    std::uint32_t il2cppclass_name_ptr = 0x10;
    std::uint32_t il2cppclass_namespace_ptr = 0x18;
    std::uint32_t il2cppclass_parent = 0x58;

    std::uint32_t ms_id_set_entries_base = 0x00;
    std::uint32_t ms_id_set_count = 0x08;

    std::uint32_t ms_id_entry_key = 0x00;
    std::uint32_t ms_id_entry_object = 0x10;
    std::uint32_t ms_id_entry_stride = 0x18;
};

} // namespace er6
