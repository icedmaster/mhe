#include "platform/opengl3/opengl3_atomics.hpp"

#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_helpers.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3Atomic::init(int type)
{
    return vbo_.init(GL_ATOMIC_COUNTER_BUFFER, get_size_by_type(type), nullptr, GL_DYNAMIC_DRAW);
}

void OpenGL3Atomic::destroy()
{
    vbo_.close();
}

void OpenGL3Atomic::bind(size_t unit) const
{
    vbo_.enable();
    OpenGLExtensions::instance().glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, unit, vbo_.id());
    CHECK_GL_ERRORS();
}

void OpenGL3Atomic::update(const uint8_t* value, size_t size)
{
    vbo_.update(size, 0, value);
}

size_t OpenGL3Atomic::get_size_by_type(int type) const
{
    UNUSED(type);
    return sizeof(uint32_t);
}
}}
