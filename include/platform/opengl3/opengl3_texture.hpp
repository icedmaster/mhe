#ifndef __OPENGL3_TEXTURE_HPP__
#define __OPENGL3_TEXTURE_HPP__

#include "render/texture.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class OpenGL3Texture : public TextureImpl
{
public:
	bool init(const TextureDesc& desc, const uint8_t* data, size_t size);
	void close();

	void enable(size_t unit) const;
	void disable() const;

	GLuint id() const
	{
		return id_;
	}

	void update(const uint8_t* data) override;

	void copy_framebuffer() override;
private:
	void init_cubemap(const TextureDesc& desc, const uint8_t* data, size_t size);

	GLuint id_;
	GLenum target_;
	GLsizei width_;
	GLsizei height_;
	GLenum format_;
	GLenum datatype_;
	mutable size_t unit_;
};

}}

#endif
