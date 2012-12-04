#include "platform/opengles/opengles_driver.hpp"

#include "platform/opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

void OpenGLESDriver::clear_depth_impl()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLESDriver::clear_color_impl()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLESDriver::set_clear_color_impl(const colorf& color)
{
	glClearColor(color.r(), color.g(), color.b(), color.a());
}

}}
