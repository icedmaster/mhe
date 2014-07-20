#include "render/shader_program.hpp"

#include "impl/system_factory.hpp"
#include "utils/strutils.hpp"

namespace mhe {

ShaderProgram::ShaderProgram() :
	impl_(SystemFactory::instance().create_shader_program())
{}

bool ShaderProgram::init(const std::string& vsdata, const std::string& fsdata)
{
	return impl_->init(vsdata, fsdata);
}

}
