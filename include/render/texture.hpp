#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "core/unique_ptr.hpp"

namespace mhe {

struct TextureDesc
{
};

class TextureImpl
{
public:
	virtual ~TextureImpl() {}
	virtual bool init(const TextureDesc& desc) = 0;
	virtual void close() = 0;
};

class Texture
{
public:
	Texture();

	bool init(const TextureDesc& desc)
	{
		return impl_->init(desc);
	}

	void close()
	{
		impl_->close();
	}
private:
	unique_ptr<TextureImpl> impl_;
};

}

#endif
