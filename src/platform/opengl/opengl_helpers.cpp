#include "platform/opengl/opengl_helpers.hpp"

#include "core/hashmap.hpp"
#include "core/types_cast.hpp"

namespace mhe {
namespace opengl {

namespace {
hashmap<size_t, string> enum_to_string_map;
}

#define INIT_DESCRIPTION(e) enum_to_string_map[e] = #e

void init_text_description()
{
    ASSERT(enum_to_string_map.empty(), "The descriptions have already been initialized");

    INIT_DESCRIPTION(GL_INVALID_ENUM);
    INIT_DESCRIPTION(GL_INVALID_VALUE);
    INIT_DESCRIPTION(GL_INVALID_OPERATION);
    INIT_DESCRIPTION(GL_STACK_OVERFLOW);
    INIT_DESCRIPTION(GL_STACK_UNDERFLOW);
    INIT_DESCRIPTION(GL_OUT_OF_MEMORY);
    INIT_DESCRIPTION(GL_INVALID_FRAMEBUFFER_OPERATION);

    INIT_DESCRIPTION(GL_VERTEX_SHADER);
    INIT_DESCRIPTION(GL_FRAGMENT_SHADER);
    INIT_DESCRIPTION(GL_GEOMETRY_SHADER);
    INIT_DESCRIPTION(GL_COMPUTE_SHADER);
}

string enum_to_string(GLenum e)
{
    size_t value = static_cast<size_t>(e);
    auto it = enum_to_string_map.find(value);
    if (it == enum_to_string_map.end())
        return types_cast<string>(value);
    return it->value;
}

}}
