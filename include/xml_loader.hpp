#ifndef __XML_LOADER_HPP__
#define __XML_LOADER_HPP__

#include "utils/loader_utils.hpp"
#include "texture_manager.hpp"
#include "sprite.hpp"

namespace mhe {

class XMLLoader
{
	struct SpriteDummyLoader
	{
		typedef Sprite type;
		typedef void* helper_type;
		static Sprite* load(const std::string&, const helper_type&) { return nullptr; }
	};
public:
	XMLLoader(const std::string& filename,
			  TextureManager& texture_manager);
	XMLLoader(const pugi::xml_node& node,
			  TextureManager& texture_manager);

	boost::shared_ptr<Sprite> load_sprite(const std::wstring& name);
	boost::shared_ptr<Texture> load_texture(const std::wstring& name);
private:
	boost::shared_ptr<Sprite> load_sprite_impl(const pugi::xml_node& node);
	AnimationList read_animation_list(const pugi::xml_node& node);
	Animation read_animation(const pugi::xml_node& node);

	pugi::xml_document doc_;
	pugi::xml_node root_;
	pugi::xml_node textures_node_;
	TextureManager& texture_manager_;		
	ResourceManager<SpriteDummyLoader> sprite_manager_;
};

}

#endif
