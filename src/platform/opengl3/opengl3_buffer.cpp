#include "platform/opengl3/opengl3_buffer.hpp"

#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

OpenGL3VBO::OpenGL3VBO() :
	id_(0)
{}

OpenGL3VBO::~OpenGL3VBO()
{
	destroy_buffer();
}

bool OpenGL3VBO::init(const float* data, size_t size)
{
	return init(GL_ARRAY_BUFFER, size, reinterpret_cast<const GLvoid*>(data), GL_STATIC_DRAW);
}

bool OpenGL3VBO::init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	if (!id_)
	{
		OpenGLExtensions::instance().glGenBuffers(1, &id_);
		if (!id_) return false;
	}
	OpenGLExtensions::instance().glBindBuffer(target, id_);
	OpenGLExtensions::instance().glBufferData(target, size * sizeof(float), data, usage);
	target_ = target;
	return true;
}

void OpenGL3VBO::destroy_buffer()
{
	OpenGLExtensions::instance().glDeleteBuffers(1, &id_);
}

void OpenGL3VBO::enable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, id_);
}

void OpenGL3VBO::disable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, 0);
}

// VAO
OpenGL3VAO::OpenGL3VAO() :
	id_(0)
{}

OpenGL3VAO::~OpenGL3VAO()
{
	destroy_array();
}

bool OpenGL3VAO::init()
{
	if (!id_)
	{
		OpenGLExtensions::instance().glGenVertexArrays(1, &id_);
		if (!id_) return false;
	}
	return true;
}

void OpenGL3VAO::enable()
{
	OpenGLExtensions::instance().glBindVertexArray(id_);
}

void OpenGL3VAO::disable()
{
	OpenGLExtensions::instance().glBindVertexArray(0);
}

void OpenGL3VAO::destroy_array()
{
	OpenGLExtensions::instance().glDeleteVertexArrays(1, &id_);
}

// buffer
bool OpenGLBuffer::init_impl()
{
	if (!vao_.init()) return false;
	vao_.enable();
    const std::vector<float>& dt = data();
	if (!vbo_.init(&dt[0], dt.size())) return false;
	return true;
}

void OpenGLBuffer::enable_impl()
{
	vao_.enable();
	vbo_.enable();
}

void OpenGLBuffer::disable_impl()
{
    vbo_.disable();
	vao_.disable();
}

}}
