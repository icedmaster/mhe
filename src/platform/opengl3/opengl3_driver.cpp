#include "platform/opengl3/opengl3_driver.hpp"

#include "platform/opengl/mhe_gl.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_utils.hpp"
#include "platform/opengl3/shaders/default_shaders.hpp"

namespace mhe {
namespace opengl {

bool OpenGL3Driver::init()
{
	return init_default_shader();
}

bool OpenGL3Driver::init_default_shader()
{
	OpenGL3Shader vert(Shader::vertex_shader);
	OpenGL3Shader frag(Shader::fragment_shader);
	if (!vert.load(default_vertex_shader_data))
		return false;
	if (!frag.load(default_fragment_shader_data))
		return false;
	boost::shared_ptr<OpenGL3ShaderProgram> program(new OpenGL3ShaderProgram);
	program->set_shaders(vert, frag);
	set_shader_program(program);
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
    
void OpenGL3Driver::mask_zbuffer()
{
    glDepthMask(GL_FALSE);
}
    
void OpenGL3Driver::unmask_zbuffer()
{
    glDepthMask(GL_TRUE);
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
    
void OpenGL3Driver::set_blend_func(BlendMode bf)
{
    switch (bf)
    {
        case alpha_one_minus_alpha:
           glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
                
        default: break;
    }
}

void OpenGL3Driver::set_shader_program(const boost::shared_ptr<ShaderProgram>& program)
{
	const OpenGL3ShaderProgram* shader_program = static_cast<const OpenGL3ShaderProgram*>(program.get());
	OpenGLExtensions::instance().glUseProgram(shader_program->id_);
	active_shader_program_ = boost::static_pointer_cast<OpenGL3ShaderProgram>(program);
}
    
void OpenGL3Driver::load_projection_matrix(const matrixf& m)
{
	assert(active_shader_program_ != nullptr);
	active_shader_program_->set_uniform(projection_matrix_uniform_name, m);
}

void OpenGL3Driver::begin_draw(const RenderBuffer* buffer)
{
	check_for_errors();
	// TODO: need to apply correct texture unit
	active_shader_program_->set_uniform(texture_unit_uniform_name, 0);

    const std::vector<float>& data = buffer->data();
    size_t v_offset = buffer->offset(vertex_position_attribute_name);
    if (v_offset != RenderBuffer::invalid_offset)
        active_shader_program_->set_attribute(vertex_position_attribute_name,
                                           &data[0], 3, v_offset * sizeof(float));
    size_t n_offset = buffer->offset(vertex_normal_attribute_name);
    if (n_offset != RenderBuffer::invalid_offset)
        active_shader_program_->set_attribute(vertex_normal_attribute_name,
                                           &data[0], 3, n_offset * sizeof(float));
    size_t t_offset = buffer->offset(vertex_texcoord_attribute_name);
    if (t_offset != RenderBuffer::invalid_offset)
        active_shader_program_->set_attribute(vertex_texcoord_attribute_name,
                                           &data[0], 2, t_offset * sizeof(float));
    size_t c_offset = buffer->offset(vertex_color_attribute_name);
    if (c_offset != RenderBuffer::invalid_offset)
        active_shader_program_->set_attribute(vertex_color_attribute_name,
                                           &data[0], 4, c_offset * sizeof(float));
}
    
void OpenGL3Driver::draw(const cmn::uint* i, cmn::uint size)
{
	check_for_errors();
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, i);
}
    
void OpenGL3Driver::end_draw(const RenderBuffer* /*buffer*/)
{
	check_for_errors();
    active_shader_program_->disable_attribute(vertex_position_attribute_name);
    active_shader_program_->disable_attribute(vertex_normal_attribute_name);
    active_shader_program_->disable_attribute(vertex_texcoord_attribute_name);
    active_shader_program_->disable_attribute(vertex_color_attribute_name);
}
    
void OpenGL3Driver::set_viewport(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
}
    
void OpenGL3Driver::end_render()
{
    glFlush();
}
    
}}
