#ifndef __OPENGL3_TEXTURE_HPP__
#define __OPENGL3_TEXTURE_HPP__

#include "render/texture.hpp"
#include "../opengl/mhe_gl.hpp"

namespace mhe {
namespace opengl {

class OpenGL3Texture : public TextureImpl
{
public:
	bool init(const TextureDesc& desc);
	void close();
private:
	GLuint id_;
};

}}

#endif
