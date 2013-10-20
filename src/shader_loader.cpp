#include "shader_loader.hpp"

#include "context.hpp"
#include "shader.hpp"

namespace mhe {

ShaderLoader::type* ShaderLoader::load(const std::string& name, const helper_type& helper)
{
	const std::string vertex_extension = ".vert";
	const std::string fragment_extension = ".frag";
	// TODO: need to add support for already compiled shaders
	ShaderProgram* shader_program = SystemFactory::instance().video_driver_factory().create_shader_program();
	if (shader_program == nullptr)
		return nullptr;
	if (shader_program->load_files(name + vertex_extension, name + fragment_extension))
		return shader_program;
	delete shader_program;
	return nullptr;
}

}
