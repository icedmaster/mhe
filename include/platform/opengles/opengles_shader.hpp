#ifndef __OPENGLES_SHADER_HPP__
#define __OPENGLES_SHADER_HPP__

#include "../opengl/mhe_gl.hpp"
#include "../opengl/opengl_extension.hpp"
#include "shader.hpp"

namespace mhe {
namespace opengl {

class OpenGLESShaderProgram;

class OpenGLESShader : public Shader
{
	friend class OpenGLESShaderProgram;
public:
	OpenGLESShader(ShaderType type);
	~OpenGLESShader();

	bool compile();

	bool check_status(GLenum param) const;
private:
	bool load_impl(const std::string& data);
private:
	void remove_shader();
	GLuint opengl_shader_type() const;

	GLuint id_;
};

}}

#endif
