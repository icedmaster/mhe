#ifndef __COMMON_TYPES_WRAPPER_HPP__
#define __COMMON_TYPES_WRAPPER_HPP__

#include "core/types.hpp"

namespace mhe {

namespace game {
class Engine;
}

class RDBGProcessor;

void register_common_types(RDBGProcessor& rdbg_processor);

bool transform_set_data(game::Engine& engine, const uint8_t* data, size_t size, size_t offset);
bool transform_get_data(game::Engine& engine, uint8_t* data, size_t& size, size_t offset);
bool transform_set_object_data(game::Engine& engine, uint32_t objectid, const uint8_t* data, size_t size, uint32_t field_index);
bool transform_get_object_data(game::Engine& engine, uint32_t objectid, uint8_t* data, size_t& size, uint32_t field_index);

}

#endif
