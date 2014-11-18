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

void VAO::enable() const
{
	OpenGLExtensions::instance().glBindVertexArray(id_);
}

void VAO::disable() const
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

void VBO::enable() const
{
	OpenGLExtensions::instance().glBindBuffer(target_, id_);
}

void VBO::disable() const
{
	OpenGLExtensions::instance().glBindBuffer(target_, 0);
}

void VBO::update(GLsizeiptr size, GLintptr offset, const GLvoid* data)
{
	enable();
	OpenGLExtensions::instance().glBufferSubData(target_, offset, size, data);
	CHECK_GL_ERRORS();
	disable();
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

void OpenGL3Buffer::enable() const
{
	vao_.enable();
	vbo_.enable();
}

void OpenGL3Buffer::disable() const
{
	vao_.disable();
	vbo_.disable();
}

bool OpenGL3IndexBuffer::init(const RenderBuffer& render_buffer, const uint32_t* indexes, size_t size)
{
	const OpenGL3Buffer* buffer = static_cast<const OpenGL3Buffer*>(render_buffer.impl());
	buffer->vao().enable();
	indexes_.resize(size);
	::memcpy(&indexes_[0], indexes, size * sizeof(uint32_t));
	bool result = vbo_.init(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), reinterpret_cast<const GLvoid*>(indexes), GL_STATIC_DRAW);
	buffer->vao().disable();
	return result;
}

void OpenGL3IndexBuffer::enable() const
{
	vbo_.enable();
}

bool OpenGL3Layout::init(const LayoutDesc& desc)
{
    ASSERT(!desc.layout.empty(), "Getting layout automatically is not supported yet");
	desc_ = desc;
	return true;
}

void OpenGL3Layout::enable() const
{
	for (size_t i = 0; i < desc_.layout.size(); ++i)
	{
		const LayoutElement& element = desc_.layout[i];
		OpenGLExtensions::instance().glVertexAttribPointer(element.position, element.size, GL_FLOAT, GL_FALSE,
														   element.stride, reinterpret_cast<const void*>(element.offset));
		OpenGLExtensions::instance().glEnableVertexAttribArray(element.position);
	}
}

void OpenGL3Layout::disable() const
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
	id_ = OpenGLExtensions::instance().glGetUniformBlockIndex(program->id(), desc.name);		// uniform block index
	ASSERT(id_ != GL_INVALID_INDEX, "Can't create uniform buffer");
	GLint size;
	OpenGLExtensions::instance().glGetActiveUniformBlockiv(program->id(), id_, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

	data_.resize(size);
	if (!desc.uniforms.empty())
	{
		const GLchar* names[max_uniforms_per_block];
		GLuint indices[max_uniforms_per_block];
		for (size_t i = 0; i < desc.uniforms.size(); ++i)
			names[i] = desc.uniforms[i].name;
		OpenGLExtensions::instance().glGetUniformIndices(program->id(), desc.uniforms.size(), names, indices);
		OpenGLExtensions::instance().glGetActiveUniformsiv(program->id(), desc.uniforms.size(), indices, GL_UNIFORM_OFFSET, offsets_);

		for (size_t i = 0; i < desc.uniforms.size(); ++i)
			::memcpy(data_.begin() + offsets_[i], desc.uniforms[i].data, desc.uniforms[i].size);
	}

	if (!vbo_.init(GL_UNIFORM_BUFFER, size, &data_[0], GL_DYNAMIC_DRAW))
	{
		ASSERT(false, "Can't create VBO for UniformBuffer");
		return false;
	}

	return true;
}

void OpenGL3UniformBuffer::close()
{
	vbo_.close();
}

void OpenGL3UniformBuffer::update(const UniformBufferDesc& desc)
{
#ifdef MHE_DEBUG
	size_t size = 0;
	for (size_t i = 0; i < desc.uniforms.size(); ++i)
		size += desc.uniforms[i].size;
	ASSERT(size == data_.size(), "Invalid UniformBufferDesc");
#endif
	for (size_t i = 0; i < desc.uniforms.size(); ++i)
		::memcpy(data_.begin() + offsets_[i], desc.uniforms[i].data, desc.uniforms[i].size);
	vbo_.update(data_.size(), 0, &data_[0]);
}

void OpenGL3UniformBuffer::update(const uint8_t* data, size_t offset, size_t size)
{
	vbo_.update(size, offset, data);
}

void OpenGL3UniformBuffer::bind(size_t unit) const
{
    vbo_.enable();
    OpenGLExtensions::instance().glBindBufferBase(GL_UNIFORM_BUFFER, unit, vbo_.id());
}

void OpenGL3UniformBuffer::enable(GLuint program, size_t unit) const
{
    OpenGLExtensions::instance().glUniformBlockBinding(program, id_, unit);
}

void OpenGL3UniformBuffer::disable() const
{
	vbo_.disable();
}

}}
