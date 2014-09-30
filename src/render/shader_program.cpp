#include "render/shader_program.hpp"

#include "impl/system_factory.hpp"
#include "utils/strutils.hpp"

namespace mhe {

ShaderProgram::ShaderProgram() :
	impl_(SystemFactory::instance().create_shader_program())
{}

bool ShaderProgram::init(const std::string& vsdata, const std::string& fsdata, const ShaderInitializationParams& params)
{
	return impl_->init(vsdata, fsdata, params);
}

void UberShader::Index::set(const Info& info, size_t value)
{
	index_ += calculate_index(info, value);
}

size_t UberShader::Index::calculate_index(const Info& info, size_t value)
{
	return info.offset * (value - info.low);
}

void UberShader::add(size_t value, ShaderProgram::IdType id)
{
	if (variants_.size() < value + 1 || variants_.empty())
		variants_.resize(value + 1);
	variants_[value] = id;
}

ShaderProgram::IdType UberShader::get(const Index& index) const
{
	size_t idx = index.index();
	ASSERT(idx < variants_.size(), "Invalid index:" << idx);
	return variants_[idx];
}

const UberShader::Info& UberShader::info(const char* name) const
{
	for (size_t i = 0; i < infos_.size(); ++i)
	{
		if (infos_[i].name == name)
			return infos_[i];
	}
	ASSERT(0, "Invalid info name:" << name);
	return infos_.front();
}

}
