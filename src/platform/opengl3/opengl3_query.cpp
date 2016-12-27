#include "platform/opengl3/opengl3_query.hpp"

#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_helpers.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3Query::init(int target)
{
    OpenGLExtensions::instance().glGenQueries(1, &id_);

    CHECK_GL_ERRORS();

    target_ = get_query_target(target);
    return true;
}

void OpenGL3Query::destroy()
{
    OpenGLExtensions::instance().glDeleteQueries(1, &id_);
    CHECK_GL_ERRORS();
}

void OpenGL3Query::begin()
{
    ASSERT(target_ != GL_TIMESTAMP, "begin() and end() methods are not support for timestamp queries");
    OpenGLExtensions::instance().glBeginQuery(target_, id_);
    CHECK_GL_ERRORS();
}

void OpenGL3Query::end()
{
    ASSERT(target_ != GL_TIMESTAMP, "begin() and end() methods are not support for timestamp queries");
    OpenGLExtensions::instance().glEndQuery(target_);
    CHECK_GL_ERRORS();
}

void OpenGL3Query::get(int& res) const
{
    ASSERT(target_ != GL_TIMESTAMP, "Only uint64_t type is supported for timestamps");
    OpenGLExtensions::instance().glGetQueryObjectiv(id_, GL_QUERY_RESULT, &res);
    CHECK_GL_ERRORS();
}

void OpenGL3Query::get(uint64_t& res) const
{
    OpenGLExtensions::instance().glGetQueryObjectui64v(id_, GL_QUERY_RESULT, &res);
    CHECK_GL_ERRORS();
}

void OpenGL3Query::set()
{
    ASSERT(target_ == GL_TIMESTAMP, "set() method is supported for timestamp queries only");
    OpenGLExtensions::instance().glQueryCounter(id_, target_);
    CHECK_GL_ERRORS();
}

}}
