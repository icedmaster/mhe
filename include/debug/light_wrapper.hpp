#ifndef __LIGHT_WRAPPER_HPP__
#define __LIGHT_WRAPPER_HPP__

#include "core/types.hpp"
#include "math/vector4.hpp"

namespace mhe {

namespace game {
class Engine;
}

void register_light(game::Engine& engine, size_t id, const std::string& name);

bool set_diffuse_color(game::Engine& engine, uint32_t id, const colorf& color);
bool get_diffuse_color(game::Engine& engine, uint32_t id, colorf& color);

}

#endif
