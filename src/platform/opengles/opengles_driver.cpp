#include "platform/opengles/opengles_driver.hpp"

#include "platform/opengl/mhe_gl.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengles/shaders/default_shaders.hpp"

namespace mhe {
namespace opengl {

bool OpenGLESDriver::init_impl()
{
	return init_default_shader();
}

bool OpenGLESDriver::init_default_shader()
{
	OpenGLESShader vert(Shader::vertex_shader);
	OpenGLESShader frag(Shader::fragment_shader);
	if (!vert.load(default_vertex_shader_data))
		return false;
	if (!frag.load(default_fragment_shader_data))
		return false;
	boost::shared_ptr<OpenGLESShaderProgram> program(new OpenGLESShaderProgram);
	program->set_shaders(vert, frag);
	set_shader_program_impl(program);
	return true;
}

void OpenGLESDriver::clear_depth_impl()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLESDriver::clear_color_impl()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
    
void OpenGLESDriver::mask_zbuffer_impl()
{
    glDepthMask(GL_FALSE);
}
    
void OpenGLESDriver::unmask_zbuffer_impl()
{
    glDepthMask(GL_TRUE);
}

void OpenGLESDriver::set_clear_color_impl(const colorf& color)
{
	glClearColor(color.r(), color.g(), color.b(), color.a());
}
    
void OpenGLESDriver::enable_blending_impl()
{
    glEnable(GL_BLEND);
}
    
void OpenGLESDriver::disable_blending_impl()
{
    glDisable(GL_BLEND);
}
    
void OpenGLESDriver::set_blend_func(BlendMode bf)
{
    switch (bf)
    {
        case alpha_one_minus_alpha:
           glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
                
        default: break;
    }
}

void OpenGLESDriver::set_shader_program_impl(const boost::shared_ptr<ShaderProgram>& program)
{
	const OpenGLESShaderProgram* shader_program = static_cast<const OpenGLESShaderProgram*>(program.get());
	OpenGLExtensions::instance().glUseProgram(shader_program->id_);
	active_shader_program_ = boost::static_pointer_cast<OpenGLESShaderProgram>(program);
}
    
void OpenGLESDriver::load_projection_matrix(const matrixf& m)
{
	assert(active_shader_program_ != nullptr);
	active_shader_program_->set_uniform(projection_matrix_uniform_name, m);
}

void OpenGLESDriver::begin_draw_impl(const float* v, const float* n, const float* t, const float* c,
									 cmn::uint /*size*/)
{
	active_shader_program_->set_attribute(vertex_position_attr_name, v, 3);
	active_shader_program_->set_attribute(normals_coord_attr_name, n, 3);
	active_shader_program_->set_attribute(texture_coord_attr_name, t, 2);
	active_shader_program_->set_attribute(vertex_color_attr_name, c, 4);
}
    
void OpenGLESDriver::draw_impl(const cmn::uint* i, cmn::uint size)
{
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, i);
}
    
void OpenGLESDriver::end_draw_impl()
{
    active_shader_program_->disable_attribute(vertex_position_attr_name);
    active_shader_program_->disable_attribute(normals_coord_attr_name);
    active_shader_program_->disable_attribute(texture_coord_attr_name);
    active_shader_program_->disable_attribute(vertex_color_attr_name);
}
    
void OpenGLESDriver::set_viewport_impl(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
}
    
void OpenGLESDriver::end_render_impl()
{
    glFlush();
}
    
}}
