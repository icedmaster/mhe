#ifndef __RESOURCE_LOADER_HPP__
#define __RESOURCE_LOADER_HPP__

#include "mhe_sound.hpp"
#include "utils/file_utils.hpp"
#include "impl/system_factory.hpp"

// texture
#include "itexture.hpp"
#include "png_image.hpp"
#include "siwa_image.hpp"
#include "bmp_image.hpp"

// font
#include "mhe_gui.hpp"

namespace mhe
{

struct SoundLoader
{
	typedef iSound type;
	typedef iAudioDriver helper_type;
	static type* load(const std::string& name, boost::shared_ptr<helper_type> helper);
};

struct TextureLoader
{
	typedef iTexture type;
	typedef iDriver helper_type;
	static type* load(const std::string& name, boost::shared_ptr<helper_type> helper);
};

struct FontLoader
{
	typedef gui::iFont type;
	typedef iDriver helper_type;
	static type* load(const std::string& name, boost::shared_ptr<helper_type> helper);
};

}	// namespace

#endif	// __RESOURCE_LOADER_HPP__
