#ifndef __OPENGL3_SHADER_PROGRAM_HPP__
#define __OPENGL3_SHADER_PROGRAM_HPP__

#include <map>
#include "../opengl/mhe_gl.hpp"
#include "opengl3_shader.hpp"
#include "shader_program.hpp"
#include "mhe_math.hpp"

namespace mhe {
namespace opengl {

class OpenGL3Driver;

class OpenGL3ShaderProgram : public ShaderProgram
{
	friend class OpenGL3Driver;
public:
	OpenGL3ShaderProgram();
	~OpenGL3ShaderProgram();

	bool load_files(const std::string& vertex_filename, const std::string& fragment_filename);
	bool set_shaders(const Shader& vertex_shader, const Shader& fragment_shader);

	bool check_status(GLenum param) const;

	bool set_uniform(const std::string& name, const matrixf& value);
	bool set_uniform(const std::string& name, int value);
	bool set_attribute(const std::string& name, const v3d& value);
	bool set_attribute(const std::string& name, const vector4<float>& value);
	bool set_attribute(const std::string& name, const float* data, cmn::uint size);
	bool set_attribute(const std::string& name, const float* data, cmn::uint size,
					   size_t offset, size_t stride = 0);
    bool disable_attribute(const std::string& name);
private:
	void remove_program();
	bool attach_shaders();

	GLuint id_;
	OpenGL3Shader vertex_;
	OpenGL3Shader fragment_;
};

}}

#endif