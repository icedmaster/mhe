#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "core/unique_ptr.hpp"
#include "utils/pool_utils.hpp"
#include "definitions.hpp"

namespace mhe {

// texture type (target)
enum
{
	texture_2d,
	texture_cube
};

// filter type
enum
{
	texture_filter_linear
};

// texture address mode
enum
{
	texture_wrap,
	texture_clamp,
	texture_clamp_to_edge
};

struct TextureDesc
{
	int type;
	uint width;
	uint height;
	int mag_filter;
	int min_filter;
	int address_mode_t;
	int address_mode_s;
	int format;
	int datatype;

	TextureDesc() :
		type(texture_2d), mag_filter(texture_filter_linear), min_filter(texture_filter_linear),
		address_mode_t(texture_wrap), address_mode_s(texture_wrap),
		format(format_rgba), datatype(format_ubyte)
	{}
};

class TextureImpl
{
public:
	virtual ~TextureImpl() {}
	virtual bool init(const TextureDesc& desc, const uint8_t* data, size_t size) = 0;
	virtual void close() = 0;
};

class MHE_EXPORT Texture
{
	POOL_ELEMENT_METHODS(uint16_t);
public:
	Texture();

	bool init(const TextureDesc& desc, const uint8_t* data, size_t size)
	{
		return impl_->init(desc, data, size);
	}

	void close()
	{
		impl_->close();
	}

	const TextureImpl* impl() const
	{
		return impl_.get();
	}
private:
	unique_ptr<TextureImpl> impl_;
};

struct TextureInstance
{
	Texture::IdType id;
	TextureInstance() : id(Texture::invalid_id) {}
};

}

#endif
