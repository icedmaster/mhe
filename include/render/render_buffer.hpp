#ifndef __RENDER_BUFFER_HPP__
#define __RENDER_BUFFER_HPP__

#include "core/unique_ptr.hpp"
#include "core/fixed_size_vector.hpp"
#include "utils/pool_utils.hpp"

namespace mhe {

class Driver;
class ShaderProgram;
class RenderBuffer;
struct LayoutDesc;
struct UniformBufferDesc;

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

class IndexBufferImpl
{
public:
	virtual ~IndexBufferImpl() {}
	virtual bool init(const RenderBuffer& render_buffer, const uint32_t* indexes, size_t size) = 0;
	virtual void close() = 0;
};

class LayoutImpl
{
public:
	virtual ~LayoutImpl() {}
	virtual bool init(const LayoutDesc& desc) = 0;
	virtual void close() = 0;
};

class UniformBufferImpl
{
public:
	virtual ~UniformBufferImpl() {}
	virtual bool init(const UniformBufferDesc& desc) = 0;
	virtual void close() = 0;

	virtual void update(const UniformBufferDesc& desc) = 0;
	virtual void update(const uint8_t* data, size_t offset, size_t size) = 0;
};

class MHE_EXPORT RenderBuffer
{
	friend class Driver;
    POOL_ELEMENT_METHODS(uint16_t)
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

	const RenderBufferImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<RenderBufferImpl> impl_;
};

class MHE_EXPORT IndexBuffer
{
	friend class Driver;
    POOL_ELEMENT_METHODS(uint16_t)
public:
	IndexBuffer();

	bool init(const RenderBuffer& render_buffer, const uint32_t* indexes, size_t size)
	{
		return impl_->init(render_buffer, indexes, size);
	}

	void close()
	{
		impl_->close();
	}

	const IndexBufferImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<IndexBufferImpl> impl_;
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

class MHE_EXPORT Layout
{
	friend class Driver;
    POOL_ELEMENT_METHODS(uint16_t)
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

	const LayoutImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<LayoutImpl> impl_;
};

enum UniformBufferUpdate
{
	uniform_buffer_static,
	uniform_buffer_normal
};

struct UniformBufferElement
{
	const char* name;
	size_t size;
	uint8_t* data;
};

const size_t max_uniforms_per_block = 16;
const size_t invalid_uniform_unit = static_cast<size_t>(-1);

struct UniformBufferDesc
{
	UniformBufferUpdate update_type;
	const char* name;
	ShaderProgram* program;
	fixed_size_vector<UniformBufferElement, max_uniforms_per_block> uniforms;
	size_t unit;	// fixed uniform binding index
	size_t size;	// fixed uniform size

	UniformBufferDesc() : unit(invalid_uniform_unit), size(0) {}
};

template <class T>
inline void create_uniform_buffer_element(UniformBufferDesc& desc, const char* name, const T& data)
{
	fixed_size_vector<UniformBufferElement, max_uniforms_per_block>::iterator it = desc.uniforms.next_predefined_element();
	ASSERT(it != desc.uniforms.end(), "Too many uniform elements");
	size_t size = sizeof(T);
	uint8_t* dataptr = (uint8_t*)(&data);
	it->name = name; it->size = size; it->data = dataptr;
}

class MHE_EXPORT UniformBuffer
{
	friend class Driver;
    POOL_ELEMENT_METHODS(uint16_t)
public:
	UniformBuffer();
	bool init(const UniformBufferDesc& desc)
	{
		return impl_->init(desc);
	}

	void close()
	{
		impl_->close();
	}

	void update(const UniformBufferDesc& desc)
	{
		impl_->update(desc);
	}

	void update(const uint8_t* data, size_t size)
	{
		impl_->update(data, 0, size);
	}

	void update(const uint8_t* data, size_t offset, size_t size)
	{
		impl_->update(data, offset, size);
	}

	template <class Data>
	void update(const Data& data)
	{
		impl_->update(reinterpret_cast<const uint8_t*>(&data), 0, sizeof(Data));
	}

	const UniformBufferImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<UniformBufferImpl> impl_;
};

typedef RenderBuffer VertexBuffer;

}

#endif
