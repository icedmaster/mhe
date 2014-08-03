#ifndef __OPENGL3_BUFFER_HPP__
#define __OPENGL3_BUFFER_HPP__

#include "render/render_buffer.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class VAO
{
public:
	bool init();
	void close();
	void enable() const;
	void disable() const;

	GLuint id() const
	{
		return id_;
	}
private:
	GLuint id_;
};

class VBO
{
public:
	bool init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
	void close();
	void enable() const;
	void disable() const;

	GLuint id() const
	{
		return id_;
	}

	void update(GLsizeiptr size, const GLvoid* data);
private:
	GLuint id_;
	GLenum target_;
};

class OpenGL3Buffer : public RenderBufferImpl
{
public:
	bool init(BufferUpdateType type, const uint8_t* data, size_t size, size_t element_size);
	void close();

	const VAO& vao() const
	{
		return vao_;
	}

	const VBO& vbo() const
	{
		return vbo_;
	}

	void enable() const;
	void disable() const;
private:
	VAO vao_;
	VBO vbo_;
};

class OpenGL3IndexBuffer : public IndexBufferImpl
{
public:
	bool init(const RenderBuffer& render_buffer, const uint32_t* indexes, size_t size);
	void close() {}
	
	const uint32_t* get() const
	{
		return indexes_.data();
	}

	size_t size() const
	{
		return indexes_.size();
	}

	void enable() const;
private:
	std::vector<uint32_t> indexes_;
	VBO vbo_;
};

class OpenGL3Layout : public LayoutImpl
{
public:
	bool init(const LayoutDesc& desc);
	void close() {}

	void enable() const;
	void disable() const;
private:
	LayoutDesc desc_;
};

class OpenGL3UniformBuffer : public UniformBufferImpl
{
public:
	bool init(const UniformBufferDesc& desc);
	void close();

	void update(const UniformBufferDesc& desc);

	void enable(GLuint program) const;
	void disable() const;
private:
	fixed_size_vector<uint8_t, max_uniforms_per_block * 4 * sizeof(float)> data_;
	VBO vbo_;
	GLint offsets_[max_uniforms_per_block];
	GLuint id_;
};

}}

#endif
