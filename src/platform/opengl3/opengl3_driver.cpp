#include "platform/opengl3/opengl3_driver.hpp"

#include "platform/opengl/mhe_gl.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_types.hpp"

#include "platform/opengl3/opengl3_shader_program.hpp"
#include "platform/opengl3/opengl3_render_state.hpp"
#include "platform/opengl3/opengl3_buffer.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3Driver::init()
{
	// print information about renderer and context
	INFO_LOG("OpenGL3Driver::renderer:" << reinterpret_cast<const char*>(glGetString(GL_RENDERER))); 
	INFO_LOG("OpenGL3Driver::vendor:" << reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	INFO_LOG("OpenGL3Driver::version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	int major = 0, minor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	INFO_LOG("OpenGL3Driver::context:" << major << "." << minor);
	OpenGLExtensions::instance().init_extensions();
	return true;
}

void OpenGL3Driver::clear_depth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGL3Driver::clear_color()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL3Driver::set_clear_color(const colorf& color)
{
	glClearColor(color.r(), color.g(), color.b(), color.a());
}
    
void OpenGL3Driver::enable_blending()
{
    glEnable(GL_BLEND);
}
    
void OpenGL3Driver::disable_blending()
{
    glDisable(GL_BLEND);
}
    
void OpenGL3Driver::set_viewport(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
}

void OpenGL3Driver::flush()
{
	glFlush();
}

void OpenGL3Driver::set_state(const RenderState& /*state*/)
{
}

void OpenGL3Driver::set_shader_program(const ShaderProgram& program)
{
	const OpenGL3ShaderProgram* active_program = static_cast<const OpenGL3ShaderProgram*>(program.impl());
	active_program->set();
	CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_vertex_buffer(const RenderBuffer& vbuffer)
{
	const OpenGL3Buffer* buffer = static_cast<const OpenGL3Buffer*>(vbuffer.impl());
	buffer->enable();
	CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_index_buffer(const IndexBuffer& ibuffer)
{
	const OpenGL3IndexBuffer* buffer = static_cast<const OpenGL3IndexBuffer*>(ibuffer.impl());
	current_index_buffer_ = buffer;
}

void OpenGL3Driver::set_uniform(const UniformBuffer& uniform)
{
	const OpenGL3UniformBuffer* buffer = static_cast<const OpenGL3UniformBuffer*>(uniform.impl());
	buffer->enable();
	CHECK_GL_ERRORS();
}

void OpenGL3Driver::set_layout(const Layout& layout)
{
	const OpenGL3Layout* active_layout = static_cast<const OpenGL3Layout*>(layout.impl());
	active_layout->enable();
	CHECK_GL_ERRORS();
}

void OpenGL3Driver::draw(const RenderData& data)
{
	glDrawElements(get_primitive_type(data.primitive), current_index_buffer_->size(), GL_UNSIGNED_INT, current_index_buffer_->get());
	CHECK_GL_ERRORS();
}
    
}}
