#ifndef __SHADER_PROGRAM_LOADER_HPP__
#define __SHADER_PROGRAM_LOADER_HPP__

#include "render/shader_program.hpp"

namespace mhe {

struct Context;

struct ShaderProgramLoader
{
	typedef Shader type;
	typedef Context context_type;
    static bool MHE_EXPORT load(type& res, const std::string& name, const context_type* context);
};

}

#endif
