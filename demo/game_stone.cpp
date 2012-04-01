#include "game_stone.hpp"
#include "game/node_aspect.hpp"
#include "utils/logutils.hpp"

void StoneFactory::init(mhe::game::mhe_loader& loader,
						const std::map<int, std::string>& stones)
{
	for (std::map<int, std::string>::const_iterator it = stones.begin();
		 it != stones.end(); ++it)
	{
		boost::shared_ptr<mhe::Sprite> sprite(loader.getSprite(mhe::utils::to_wstring(it->second)));
		if (sprite == nullptr)
		{
			WARN_LOG("StoneFactory cannot load sprite: " << it->second);
		}
		else
		{
			INFO_LOG("StoneFactory: load sprite:" << sprite->name() << " type:" << it->first);
		}
		sprites_[it->first] = sprite;
		available_types_.push_back(it->first);
	}
	engine_ = loader.get_engine();
}

boost::shared_ptr<mhe::game::Aspect> StoneFactory::create_stone(int type,
																const mhe::v3d& pos) const
{
	stone_sprite_map::const_iterator it = sprites_.find(type);
	if (it == sprites_.end())
	{
		ERROR_LOG("StoneFactory::create_stone stone for type " << type << " not found");
		return boost::shared_ptr<mhe::game::Aspect>();
	}
	boost::shared_ptr<mhe::Node> sprite(it->second->clone());
	sprite->translate(pos);
	const std::string name = "stone" + mhe::utils::to_string(get_next_id());
	boost::shared_ptr<mhe::game::NodeAspect> node(
		new mhe::game::NodeAspect(name, "node", sprite, engine_->get_game_scene()->getScene()));
	engine_->get_aspect_manager().add(node);
	return node;
}
