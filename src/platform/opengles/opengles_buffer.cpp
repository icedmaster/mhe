#include "platform/opengles/opengles_buffer.hpp"

#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

OpenGLESVBO::OpenGLESVBO() :
	id_(0)
{}

OpenGLESVBO::~OpenGLESVBO()
{
	destroy_buffer();
}

bool OpenGLESVBO::init(const float* data, size_t size)
{
	return init(GL_ARRAY_BUFFER, size, reinterpret_cast<const GLvoid*>(data), GL_STATIC_DRAW);
}

bool OpenGLESVBO::init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
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

void OpenGLESVBO::destroy_buffer()
{
	OpenGLExtensions::instance().glDeleteBuffers(1, &id_);
}

void OpenGLESVBO::enable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, id_);
}

void OpenGLESVBO::disable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, 0);
}

// VAO
OpenGLESVAO::OpenGLESVAO() :
	id_(0)
{}

OpenGLESVAO::~OpenGLESVAO()
{
	destroy_array();
}

bool OpenGLESVAO::init()
{
	if (!id_)
	{
		OpenGLExtensions::instance().glGenVertexArrays(1, &id_);
		if (!id_) return false;
	}
	return true;
}

void OpenGLESVAO::enable()
{
	OpenGLExtensions::instance().glBindVertexArray(id_);
}

void OpenGLESVAO::disable()
{
	OpenGLExtensions::instance().glBindVertexArray(0);
}

void OpenGLESVAO::destroy_array()
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
