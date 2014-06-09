#include "platform/opengl3/opengl3_buffer.hpp"

#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_extension.hpp"
#include "platform/opengl3/opengl3_shader_program.hpp"

namespace mhe {
namespace opengl {

bool VAO::init()
{
	OpenGLExtensions::instance().glGenVertexArrays(1, &id_);
    return true;
}

void VAO::close()
{
	OpenGLExtensions::instance().glDeleteVertexArrays(1, &id_);
}

void VAO::enable()
{
	OpenGLExtensions::instance().glBindVertexArray(id_);
}

void VAO::disable()
{
	OpenGLExtensions::instance().glBindVertexArray(0);
}

bool VBO::init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    OpenGLExtensions::instance().glGenBuffers(1, &id_);
	if (!id_) return false;
	OpenGLExtensions::instance().glBindBuffer(target, id_);
	OpenGLExtensions::instance().glBufferData(target, size, data, usage);
	target_ = target;
	return true;
}

void VBO::close()
{
	OpenGLExtensions::instance().glDeleteBuffers(1, &id_);
}

void VBO::enable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, id_);
}

void VBO::disable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, 0);
}

bool OpenGL3Buffer::init(BufferUpdateType type, const uint8_t* data, size_t size, size_t /*element_size*/)
{
	if (!vao_.init())
		return false;
	vao_.enable();
	bool result = vbo_.init(GL_ARRAY_BUFFER, size, data, get_vbo_usage(type));
	vao_.disable();
	return result;
}

void OpenGL3Buffer::close()
{
	vbo_.close();
	vao_.close();
}

bool OpenGL3IndexBuffer::init(const uint32_t* indexes, size_t size)
{
	indexes_.resize(size);
	::memcpy(&indexes_[0], indexes, size * sizeof(uint32_t));
	return true;
}

bool OpenGL3Layout::init(const LayoutDesc& desc)
{
    ASSERT(!desc.layout.empty(), "Getting layout automatically is not supported yet");
	desc_ = desc;
	return true;
}

void OpenGL3Layout::enable()
{
	for (size_t i = 0; i < desc_.layout.size(); ++i)
	{
		const LayoutElement& element = desc_.layout[i];
		OpenGLExtensions::instance().glVertexAttribPointer(element.position, element.size, GL_FLOAT, GL_FALSE,
														   element.stride, reinterpret_cast<const void*>(element.offset));
		OpenGLExtensions::instance().glEnableVertexAttribArray(element.position);
	}
}

void OpenGL3Layout::disable()
{
	for (size_t i = 0; i < desc_.layout.size(); ++i)
	{
		const LayoutElement& element = desc_.layout[i];
		OpenGLExtensions::instance().glDisableVertexAttribArray(element.position);
	}
}

bool OpenGL3UniformBuffer::init(const UniformBufferDesc& desc)
{
	const OpenGL3ShaderProgram* program = static_cast<const OpenGL3ShaderProgram*>(desc.program->impl());
	id_ = OpenGLExtensions::instance().glGetUniformBlockIndex(program->id(), desc.name.c_str());
	ASSERT(id_ != GL_INVALID_INDEX, "Can't create uniform buffer");
	GLint size;
	OpenGLExtensions::instance().glGetActiveUniformBlockiv(program->id(), id_, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
	const GLchar* names[max_uniforms_per_block];
	GLuint indices[max_uniforms_per_block];
	for (size_t i = 0; i < desc.uniforms.size(); ++i)
		names[i] = desc.uniforms[i].name.c_str();
	OpenGLExtensions::instance().glGetUniformIndices(program->id(), desc.uniforms.size(), names, indices);
	OpenGLExtensions::instance().glGetActiveUniformsiv(program->id(), desc.uniforms.size(), indices, GL_UNIFORM_OFFSET, offsets_);

	data_.resize(size);
	for (size_t i = 0; i < desc.uniforms.size(); ++i)
		::memcpy(data_.begin() + offsets_[i], desc.uniforms[i].data, desc.uniforms[i].size);

	if (vbo_.init(GL_UNIFORM_BUFFER, size, &data_[0], GL_DYNAMIC_DRAW))
	{
		ASSERT(false, "Can't create VBO for UniformBuffer");
		return false;
	}
	
	OpenGLExtensions::instance().glBindBufferBase(GL_UNIFORM_BUFFER, id_, vbo_.id());

	return false;
}

void OpenGL3UniformBuffer::close()
{
	vbo_.close();
}

void OpenGL3UniformBuffer::update(const UniformBufferDesc& desc)
{
}

void OpenGL3UniformBuffer::enable()
{
	vbo_.enable();
}

void OpenGL3UniformBuffer::disable()
{
	vbo_.disable();
}

}}
