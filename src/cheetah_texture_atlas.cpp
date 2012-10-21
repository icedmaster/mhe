#include "cheetah_texture_atlas.hpp"

#include <fstream>
#include "utils/strutils.hpp"
#include "utils/file_utils.hpp"

namespace mhe {

TextureAtlas* CheetahTextureAtlas::load(const std::string& filename)
{
	if (!load_texture(filename))
		return nullptr;
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
		atlas->set_texture(texture_);
	}
	else
	{
		// TODO: remove texture from manager
	}
	return atlas;
}

bool CheetahTextureAtlas::load_texture(const std::string& filename)
{
	const std::string cheetah_texture_extension = ".png";
	std::string texture_filename = utils::get_file_name_with_path(filename) +
		cheetah_texture_extension;
	texture_ = texture_manager_->get(texture_filename);
	return (texture_ != nullptr);
}

TextureAtlas* CheetahTextureAtlas::load_atlas_from_file(std::ifstream& stream)
{
	TextureAtlas* atlas = new TextureAtlas;
	// we need to ignore first line in file. It contains texture filenames, but we
	// think that texture has same name as atlas.
	bool first_line = true;	
	while (!stream.eof())
	{
		std::string str;
		std::getline(stream, str);
		if (str.empty()) continue;
		if (first_line)
		{
			first_line = false;
			continue;
		}
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
	const std::string& str_x = params[1];
	const std::string& str_y = params[2];
	const std::string& str_width = params[3];
	const std::string& str_height = params[4];
	float x = utils::to_number(str_x);
	float y = utils::to_number(str_y);
	float width = utils::to_number(str_width);
	float height = utils::to_number(str_height);
	rect<float> r(x, texture_->height() - height - y, width, height);
	atlas->add(name, r);
}

}
