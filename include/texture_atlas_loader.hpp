#ifndef __TEXTURE_ATLAS_LOADER_HPP__
#define __TEXTURE_ATLAS_LOADER_HPP__

#include "texture_atlas.hpp"
#include "texture_manager.hpp"

namespace mhe {

struct TextureAtlasLoader
{
	typedef TextureAtlas type;
	typedef TextureManager* helper_type;
	static type* load(const std::string& name, const helper_type& helper);
};

}

#endif
