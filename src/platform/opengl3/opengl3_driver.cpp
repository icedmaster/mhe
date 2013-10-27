#include "platform/opengl3/opengl3_driver.hpp"

#include "platform/opengl/mhe_gl.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl/opengl_utils.hpp"
#include "platform/opengl/opengl_texture.hpp"
#include "platform/opengl3/shaders/default_shaders.hpp"

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
	load_projection_matrix(active_projection_matrix_);
}
    
void OpenGL3Driver::load_projection_matrix(const matrixf& m)
{
	assert(active_shader_program_ != nullptr);
	active_shader_program_->set_uniform(projection_matrix_uniform_name, m);
	active_projection_matrix_ = m;
}

void OpenGL3Driver::begin_draw(const RenderBuffer* buffer, const material_ptr* materials, size_t materials_number)
{
	assert(buffer != nullptr);
	assert(materials_number > 0);
	assert(materials != nullptr);
	CHECK_FOR_ERRORS();
	// update shader - we presume that all materials for one render call are using the same shader
	set_shader_program(materials[0]->shader());
	size_t total_textures_number = 0;
	for (size_t i = 0; i < materials_number; ++i)
	{
		for (size_t j = 0; j < materials[i]->textures_number(); ++j)
		{
			OpenGLTexture* texture = static_cast<OpenGLTexture*>(materials[i]->texture_at(j).get());
			OpenGLExtensions::instance().glActiveTexture(GL_TEXTURE0 + total_textures_number);
			texture->prepare();
			active_shader_program_->set_uniform(utils::name_for_texture_unit(total_textures_number),
				total_textures_number); 
			const std::string& attrib_name = utils::name_for_texture_attribute(total_textures_number);
			size_t t_offset = buffer->offset(attrib_name);
			if (t_offset != RenderBuffer::invalid_offset)
				active_shader_program_->set_attribute(attrib_name,
													  nullptr, 2, t_offset * sizeof(float));   
			++total_textures_number;
		}
	}
    size_t v_offset = buffer->offset(vertex_position_attribute_name);
    if (v_offset != RenderBuffer::invalid_offset)
        active_shader_program_->set_attribute(vertex_position_attribute_name,
                                           nullptr, 3, v_offset * sizeof(float));
    size_t n_offset = buffer->offset(vertex_normal_attribute_name);
    if (n_offset != RenderBuffer::invalid_offset)
        active_shader_program_->set_attribute(vertex_normal_attribute_name,
                                           nullptr, 3, n_offset * sizeof(float));
    size_t c_offset = buffer->offset(vertex_color_attribute_name);
    if (c_offset != RenderBuffer::invalid_offset)
        active_shader_program_->set_attribute(vertex_color_attribute_name,
                                           nullptr, 4, c_offset * sizeof(float));
}
    
void OpenGL3Driver::draw(const cmn::uint* i, cmn::uint size)
{
	CHECK_FOR_ERRORS();
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, i);
}
    
void OpenGL3Driver::end_draw(const RenderBuffer* /*buffer*/, size_t materials_number)
{
	CHECK_FOR_ERRORS();
    active_shader_program_->disable_attribute(vertex_position_attribute_name);
    active_shader_program_->disable_attribute(vertex_normal_attribute_name);
    active_shader_program_->disable_attribute(vertex_color_attribute_name);
	for (size_t i = 0; i < materials_number; ++i)
		active_shader_program_->disable_attribute(utils::name_for_texture_attribute(i));
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
