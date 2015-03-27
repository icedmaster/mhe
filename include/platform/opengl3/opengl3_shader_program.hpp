#ifndef __OPENGL3_SHADER_PROGRAM_HPP__
#define __OPENGL3_SHADER_PROGRAM_HPP__

#include "render/shader_program.hpp"
#include "../opengl/mhe_gl.hpp"
#include "core/array.hpp"

namespace mhe {
namespace opengl {

class OpenGL3ShaderProgram : public ShaderProgramImpl
{

class Shader
{
public:
	~Shader()
	{
		close();
	}

	bool init(GLenum type, const std::string& data);
	void close();

	GLuint id() const
	{
		return id_;
	}
private:
    bool check_status(GLenum type, GLenum param) const;

	GLuint id_;
};

public:
	~OpenGL3ShaderProgram()
	{
		close();
	}
	
	bool init(const std::string& vsdata, const std::string& fsdata, const ShaderInitializationParams& params);
	void close();

	GLuint id() const
	{
		return id_;
	}

	void set() const;

	GLuint uniform_location(size_t unit) const;
private:
	bool attach_shaders();
	bool check_status(GLenum param) const;
	void init(const ShaderInitializationParams& params);

	array<GLuint, 16> texture_location_;	// TODO: need to find a GL constant for this
	array<GLuint, 16> shader_bind_indexes_;
	Shader vertex_shader_;
	Shader fragment_shader_;
	GLuint id_;
};

}}

#endif
