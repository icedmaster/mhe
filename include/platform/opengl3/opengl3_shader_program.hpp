#ifndef __OPENGL3_SHADER_PROGRAM_HPP__
#define __OPENGL3_SHADER_PROGRAM_HPP__

#include "render/shader_program.hpp"
#include "../opengl/mhe_gl.hpp"

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
	bool check_status(GLenum param) const;

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
private:
	bool attach_shaders();
	bool check_status(GLenum param) const;
	void init_textures(const ShaderInitializationParams& params);

	GLuint texture_location_[16];	// TODO: need to find a GL constant for this
	Shader vertex_shader_;
	Shader fragment_shader_;
	GLuint id_;
};

}}

#endif
