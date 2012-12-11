#include "render_buffer.hpp"

namespace mhe {

bool RenderBuffer::attach_data(const std::string& name, const float* data, size_t size)
{
	if (offsets_.find(name) != offsets_.end()) return false;
	if (initialized_) return false;
	offsets_[name] = data_.size();
	data_.insert(data_.end(), data, data + size);
	return true;
}

size_t RenderBuffer::offset(const std::string& name) const
{
	std::map<std::string, size_t>::const_iterator it = offsets_.find(name);
	if (it == offsets_.end())
		return invalid_offset;
	return it->second;
}

}
