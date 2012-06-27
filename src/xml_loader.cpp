#include "xml_loader.hpp"

#include "utils/global_log.hpp"

namespace mhe {

XMLLoader::XMLLoader(const std::string& filename,
					 TextureManager& texture_manager) :
	texture_manager_(texture_manager)
{
	pugi::xml_parse_result res = doc_.load_file(filename.c_str());
	if (res.status != pugi::status_ok) return;
	root_ = doc_.root();
	textures_node_ = doc_.child(L"textures");
}

XMLLoader::XMLLoader(const pugi::xml_node& node,
					 TextureManager& texture_manager) :
	root_(node),
	textures_node_(node.child(L"textures")),
	texture_manager_(texture_manager)
{
}

boost::shared_ptr<Sprite> XMLLoader::load_sprite(const std::wstring& name)
{
	const std::string& sname = utils::from_wstr(name);
	if (sprite_manager_.have(sname))
		return sprite_manager_.get(sname);
	const pugi::xml_node& node = utils::find_node(root_, L"sprite", name);
	if (!node) return boost::shared_ptr<Sprite>();
	boost::shared_ptr<Sprite> sprite = load_sprite_impl(node);
	if (sprite != nullptr)
		sprite_manager_.add(sname, sprite);
	return sprite;
}

boost::shared_ptr<Texture> XMLLoader::load_texture(const std::wstring& name)
{
	const pugi::xml_node& node = utils::find_node(textures_node_, L"texture", name);
	if (!node) return boost::shared_ptr<Texture>();
	return texture_manager_.get(utils::from_wstr(node.child(L"file").child_value()));
}

boost::shared_ptr<Sprite> XMLLoader::load_sprite_impl(const pugi::xml_node& node)
{   
	// try read sprite position
	pugi::xml_node pos_node = node.child(L"position");
	const v3d& pos  = utils::read_vector3(pos_node);
	const v3d& size = utils::read_vector3(node.child(L"size"));
	pugi::xml_node al_node = node.child(L"animation_list");  // first

	boost::shared_ptr<Sprite> sprite(new Sprite);
	for (pugi::xml_node it = al_node; it; it = it.next_sibling(L"animation_list"))
		sprite->addAnimationList(read_animation_list(it));

	sprite->translate(pos);
	sprite->set_size(size.x(), size.y());
	sprite->set_name(utils::from_wstr(node.child_value(L"name")));
	DEBUG_LOG("Load sprite: name=" + sprite->name() + " size_x: " << size.x() << " size_y: " << size.y());
	return sprite;
}

AnimationList XMLLoader::read_animation_list(const pugi::xml_node& node)
{
	cmn::uint num = node.attribute(L"number").as_uint();
	bool repeat = node.attribute(L"repeat").as_bool();
	AnimationList a_list(repeat);
	a_list.setIndex(num);
	for (pugi::xml_node it = node.child(L"animation"); it;
		 it = it.next_sibling(L"animation"))
	{
		a_list.add(read_animation(it));
	}
	return a_list;
}

Animation XMLLoader::read_animation(const pugi::xml_node& node)
{
	Animation a(utils::read_number<cmn::uint>(node.child(L"duration")),
				load_texture(node.child(L"texture").child_value()));

	// read transformations
	for (pugi::xml_node it = node.first_child(); it; it = it.next_sibling())
	{
		if (std::wstring(it.name()) == L"translate")
			a.translate(utils::read_vector3(it));
		else if (std::wstring(it.name()) == L"rotate")
		{
			float angle = utils::read_number<float>(it, L"angle");
			const v3d& v = utils::read_vector3(it);
			a.rotate(v, angle);
		}
		else if (std::wstring(it.name()) == L"scale")
			a.scale(utils::read_vector3(it));
	}
	return a;
}

}
