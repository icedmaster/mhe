#include "render/shader_program.hpp"

#include "impl/system_factory.hpp"
#include "utils/strutils.hpp"

namespace mhe {

ShaderProgram::ShaderProgram() :
	impl_(SystemFactory::instance().create_shader_program())
{}

bool ShaderProgram::init(const std::string& vsdata, const std::string& fsdata, const std::string& definitions)
{
	const std::vector<std::string>& splitted_definitions = utils::split(definitions, ",");
	std::string def;
	for (size_t i = 0; i < splitted_definitions.size(); ++i)
		def += ("#define " + splitted_definitions[i] + "\n");
	return impl_->init(def + vsdata, def + fsdata);
}

}
