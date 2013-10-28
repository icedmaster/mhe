#ifndef __RESOURCE_LOADER_HPP__
#define __RESOURCE_LOADER_HPP__

#include "utils/file_utils.hpp"
#include "impl/system_factory.hpp"

// texture
#include "texture.hpp"
#include "png_image.hpp"
#include "siwa_image.hpp"
#include "bmp_image.hpp"

#include "sound/isound.hpp"

namespace mhe
{

static const std::string default_resource_name = "default";

struct SoundLoader
{
	typedef iSound type;
	typedef boost::shared_ptr<iAudioDriver> helper_type;
	static type* load(const std::string& name, const helper_type* helper);
};

struct TextureLoader
{
	typedef Texture type;
	typedef Driver helper_type;
	static type* load(const std::string& name, const helper_type* helper);
};

}	// namespace

#endif	// __RESOURCE_LOADER_HPP__
