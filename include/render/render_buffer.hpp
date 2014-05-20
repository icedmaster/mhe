#ifndef __RENDER_BUFFER_HPP__
#define __RENDER_BUFFER_HPP__

#include "core/unique_ptr.hpp"
#include "core/fixed_size_vector.hpp"

namespace mhe {

class Driver;
struct LayoutDesc;

enum BufferUpdateType
{
	buffer_update_type_static,
	buffer_update_type_dynamic
};

class RenderBufferImpl
{
public:
	virtual ~RenderBufferImpl() {}
	virtual bool init(BufferUpdateType type, const uint8_t* data, size_t size, size_t element_size) = 0;
	virtual void close() = 0;
};

class LayoutImpl
{
public:
	virtual ~LayoutImpl() {}
	virtual bool init(const LayoutDesc& desc) = 0;
	virtual void close() = 0;
};

class RenderBuffer
{
	friend class Driver;
public:
	RenderBuffer();
	bool init(BufferUpdateType type, const uint8_t* data, size_t size, size_t element_size)
	{
		return impl_->init(type, data, size, element_size);
	}

	void close()
	{
		impl_->close();
	}
private:
	unique_ptr<RenderBufferImpl> impl_;
};

struct LayoutElement
{
	size_t offset;
	size_t size;
	size_t stride;
	uint8_t position;
};

struct LayoutDesc
{
	fixed_size_vector<LayoutElement, 16> layout;
};

class Layout
{
	friend class Driver;
public:
	Layout();
	bool init(const LayoutDesc& desc)
	{
		return impl_->init(desc);
	}

	void close()
	{
		impl_->close();
	}
private:
	unique_ptr<LayoutImpl> impl_;
};

}

#endif
