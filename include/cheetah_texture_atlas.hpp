#ifndef __CHEETAH_TEXTURE_ATLAS_HPP__
#define __CHEETAH_TEXTURE_ATLAS_HPP__

#include "texture_atlas.hpp"
#include "texture_manager.hpp"

namespace mhe {

class CheetahTextureAtlas
{
public:
	CheetahTextureAtlas(TextureManager* texture_manager) :
	texture_manager_(texture_manager)
	{}

	TextureAtlas* load(const std::string& filename);
private:
	TextureAtlas* load_atlas_from_file(std::ifstream& stream);
	void add_texture_atlas_element(TextureAtlas* atlas,
								   const std::string& element_str) const;

	TextureManager* texture_manager_;
};

}

#endif
