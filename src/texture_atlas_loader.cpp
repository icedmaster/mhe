#include "texture_atlas_loader.hpp"

#include "cheetah_texture_atlas.hpp"

namespace mhe {

TextureAtlas* TextureAtlasLoader::load(const std::string& name,
									   TextureManager* const& helper)
{
	// currently, support only .atlas format, don't check extension
	return CheetahTextureAtlas(helper).load(name);
}

}
