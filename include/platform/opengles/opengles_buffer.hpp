#ifndef __OPENGLES_BUFFER_HPP__
#define __OPENGLES_BUFFER_HPP__

#include "../opengl/mhe_gl.hpp"
#include "render_buffer.hpp"

namespace mhe {
namespace opengl {

class OpenGLESVBO
{
public:
	OpenGLESVBO();
	~OpenGLESVBO();

	bool init(const float* data, size_t size);
	bool init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

	void enable();
	void disable();
private:
	void destroy_buffer();

	GLuint id_;
	GLenum target_;
};

class OpenGLESVAO
{
public:
	OpenGLESVAO();
	~OpenGLESVAO();

	bool init();

	void enable();
	void disable();
private:
    void destroy_array();
    
	GLuint id_;
};

class OpenGLBuffer : public RenderBuffer
{
private:
	bool init_impl();
	void enable_impl();
	void disable_impl();	
private:
	OpenGLESVAO vao_;
	OpenGLESVBO vbo_;
};

}}

#endif
