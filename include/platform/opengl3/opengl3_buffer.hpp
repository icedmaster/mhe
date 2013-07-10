#ifndef __OPENGL3_BUFFER_HPP__
#define __OPENGL3_BUFFER_HPP__

#include "../opengl/mhe_gl.hpp"
#include "render_buffer.hpp"

namespace mhe {
namespace opengl {

class OpenGL3VBO
{
public:
	OpenGL3VBO();
	~OpenGL3VBO();

	bool init(const float* data, size_t size);
	bool init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

	void enable();
	void disable();
private:
	void destroy_buffer();

	GLuint id_;
	GLenum target_;
};

class OpenGL3VAO
{
public:
	OpenGL3VAO();
	~OpenGL3VAO();

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
	OpenGL3VAO vao_;
	OpenGL3VBO vbo_;
};

}}

#endif
