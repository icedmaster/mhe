#include "render/texture.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

Texture::Texture() :
	impl_(SystemFactory::instance().create_texture())
{}

}
