#ifndef __RENDER_BUFFER_HPP__
#define __RENDER_BUFFER_HPP__

#include <vector>
#include <map>
#include <string>

namespace mhe {

class RenderBuffer
{
public:
	static const size_t invalid_offset = static_cast<size_t>(-1);
public:
	RenderBuffer() :
		initialized_(false)
	{}

	virtual ~RenderBuffer() {}

	bool attach_data(const std::string& name, const float* data, size_t size);
	size_t offset(const std::string& name) const;

	bool init()
	{
		if (!init_impl()) return false;
		data_.clear();
		return true;
	}

	void enable()
	{
		enable_impl();
	}

	void disable()
	{
		disable_impl();
	}

	const std::vector<float>& data() const
	{
		return data_;
	}
private:
	virtual bool init_impl() = 0;
	virtual void enable_impl() = 0;
	virtual void disable_impl() = 0;
private:
	std::vector<float> data_;
	std::map<std::string, size_t> offsets_;
	bool initialized_;
};

}

#endif
