#ifndef __GRAPHICS_UTILS_HPP__
#define __GRAPHICS_UTILS_HPP__

#include "context.hpp"

namespace mhe {

class Sprite;

namespace utils {

Sprite* create_sprite(const std::string& filename, const Context& context);

}}

#endif
