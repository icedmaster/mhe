#include "render/texture.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

Texture::Texture() :
	id_(invalid_id),
	impl_(SystemFactory::instance().create_texture())
{}

Texture::~Texture()
{
    SystemFactory::instance().destroy_object(impl_);
}

}
