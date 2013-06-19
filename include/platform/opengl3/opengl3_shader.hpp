#ifndef __OPENGL3_SHADER_HPP__
#define __OPENGL3_SHADER_HPP__

#include "../opengl/mhe_gl.hpp"
#include "../opengl/opengl_extension.hpp"
#include "shader.hpp"

namespace mhe {
namespace opengl {

class OpenGL3ShaderProgram;

class OpenGL3Shader : public Shader
{
	friend class OpenGL3ShaderProgram;
public:
	OpenGL3Shader(ShaderType type);
	~OpenGL3Shader();

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
