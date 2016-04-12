#ifndef __LIGHT_WRAPPER_HPP__
#define __LIGHT_WRAPPER_HPP__

#include "core/types.hpp"
#include "math/vector4.hpp"

namespace mhe {

namespace game {
class Engine;
}

class RDBGProcessor;

void register_light_type(RDBGProcessor& rdbg_processor);

bool light_set_data(game::Engine& engine, const uint8_t* data, size_t size, size_t offset);
bool light_get_data(game::Engine& engine, uint8_t* data, size_t& size, size_t offset);
bool light_set_object_data(game::Engine& engine, uint32_t objectid, const uint8_t* data, size_t size, uint32_t offset);
bool light_get_object_data(game::Engine& engine, uint32_t objectid, uint8_t* data, size_t& size, uint32_t offset);

bool set_diffuse_color(game::Engine& engine, size_t id, const colorf& color);
bool get_diffuse_color(game::Engine& engine, size_t id, colorf& color);
bool set_specular_color(game::Engine& engine, size_t id, const colorf& color);
bool get_specular_color(game::Engine& engine, size_t id, colorf& color);
bool light_set_enabled_flag(game::Engine& engine, size_t id, const uint8_t* data, size_t size);

}

#endif
