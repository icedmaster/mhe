#include "cheetah_texture_atlas.hpp"

#include <fstream>
#include "utils/strutils.hpp"
#include "utils/file_utils.hpp"

namespace mhe {

TextureAtlas* CheetahTextureAtlas::load(const std::string& filename)
{
	std::ifstream f(filename.c_str());
	if (!f.is_open())
	{
		WARN_LOG("Can't open texture atlas file " << filename);
		return nullptr;
	}
	TextureAtlas* atlas = load_atlas_from_file(f);
	f.close();
	if (atlas != nullptr)
	{
		const std::string cheetah_texture_extension = ".png";
		std::string texture_filename = utils::get_file_name(filename) + cheetah_texture_extension;
		atlas->set_texture(texture_manager_->get(texture_filename));
	}
	return atlas;
}

TextureAtlas* CheetahTextureAtlas::load_atlas_from_file(std::ifstream& stream)
{
	TextureAtlas* atlas = new TextureAtlas;
	while (!stream.eof())
	{
		std::string str;
		std::getline(stream, str);
		if (str.empty()) continue;
		add_texture_atlas_element(atlas, str);
	}
	// add texture
	return atlas;
}

void CheetahTextureAtlas::add_texture_atlas_element(TextureAtlas* atlas,
													const std::string& element_str) const
{
	const size_t params_count = 9;
	const std::string delimiter = "\t";
	const std::vector<std::string>& params = utils::split(element_str, delimiter);
	if (params.size() < params_count) return;
	// for format see https://github.com/scriptum/Cheetah-Texture-Packer
	// Name  X pos   Y pos   Width   Height   Xoffset  Yoffset  Orig W   Orig H   Rot
	const std::string& name = params[0];
	const std::string& x = params[1];
	const std::string& y = params[2];
	const std::string& width = params[3];
	const std::string& height = params[4];
	rect<float> r(utils::to_number(x), utils::to_number(y),
				  utils::to_number(width), utils::to_number(height));
	atlas->add(name, r);
}

}
