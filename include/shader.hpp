#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <string>

namespace mhe {

class Shader
{
public:
	enum ShaderType
	{
		vertex_shader,
		fragment_shader
	};
public:
	Shader(ShaderType type) :
	type_(type)
	{}

	virtual ~Shader() {}

	ShaderType type() const
	{
		return type_;
	}

	bool load_file(const std::string& filename);
	bool load(const std::string& data)
	{
		return load_impl(data);
	}
private:
	virtual bool load_impl(const std::string& data) = 0;
private:
	ShaderType type_;
};

}

#endif
