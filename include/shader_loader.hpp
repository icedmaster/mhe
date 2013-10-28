#ifndef __SHADER_LOADER_HPP__
#define __SHADER_LOADER_HPP__

#include "shader_program.hpp"

namespace mhe {

class Context;

struct ShaderLoader
{
	typedef ShaderProgram type;
	typedef Context helper_type;
	static type* load(const std::string& name, const helper_type* helper);
};

}

#endif
