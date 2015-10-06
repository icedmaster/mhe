#include "platform/opengl3/opengl3_query.hpp"

#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_types.hpp"

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
	if (target_ == GL_TIMESTAMP) return;
	OpenGLExtensions::instance().glBeginQuery(target_, id_);
	CHECK_GL_ERRORS();
}

void OpenGL3Query::end()
{
	if (target_ == GL_TIMESTAMP) return;
	OpenGLExtensions::instance().glEndQuery(target_);
	CHECK_GL_ERRORS();
}

void OpenGL3Query::get(int& res) const
{
	NOT_IMPLEMENTED_ASSERT(target_ != GL_TIMESTAMP, "Timestamp queries");
	OpenGLExtensions::instance().glGetQueryObjectiv(id_, GL_QUERY_RESULT, &res);
	CHECK_GL_ERRORS();
}

}}
