#ifndef __SHADER_PROGRAM_HPP__
#define __SHADER_PROGRAM_HPP__

#include <string>
#include "core/unique_ptr.hpp"
#include "utils/pool_utils.hpp"

namespace mhe {

class ShaderProgramImpl
{
public:
	virtual ~ShaderProgramImpl() {}
	virtual bool init(const std::string& vsdata, const std::string& fsdata) = 0;
	virtual void close() = 0;
};

class MHE_EXPORT ShaderProgram
{
	friend class Driver;
	POOL_ELEMENT_METHODS(uint16_t);
public:
	ShaderProgram();
	bool init(const std::string& vsdata, const std::string& fsdata, const std::string& definitions = std::string());
	void close()
	{
		impl_->close();
	}

	const ShaderProgramImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<ShaderProgramImpl> impl_;
};

// resource
struct Shader
{
	uint16_t shader_program_handle;
};

}

#endif
