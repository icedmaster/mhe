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
	texture_filter_nearest,
	texture_filter_linear,
	texture_filter_nearest_mipmap_nearest,
	texture_filter_linear_mipmap_nearest,
	texture_filter_nearest_mipmap_linear,
	texture_filter_linear_mipmap_linear
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
	float anisotropic_level;
	int format;
	int datatype;
	size_t mips;

	TextureDesc() :
		type(texture_2d), mag_filter(texture_filter_nearest), min_filter(texture_filter_nearest),
		address_mode_t(texture_wrap), address_mode_s(texture_wrap), anisotropic_level(1.0f),
		format(format_rgba), datatype(format_ubyte), mips(0)
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
