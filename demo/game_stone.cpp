#include "game_stone.hpp"
#include "circle_particle_effect.hpp"
#include "test_particle_effect.hpp"
#include "game/move_aspect.hpp"

boost::shared_ptr<StoneSpriteAspect> create_stone(const mhe::game::mhe_loader& loader,
	 			  				   				  const StoneParameters& sp)
{
	mhe::game::Engine* engine = const_cast<mhe::game::Engine*>(loader.get_engine());
	
	boost::shared_ptr<mhe::iNode> sprite = engine->get_game_scene()->getScene()->get_node(sp.sprite_name);
	if (sprite == nullptr)
	{
		boost::shared_ptr<mhe::Sprite> loaded_sprite(loader.getSprite(mhe::utils::to_wstring(sp.sprite_name)));
		if (loaded_sprite != nullptr)
			engine->get_game_scene()->getScene()->add(loaded_sprite);
		loaded_sprite->translate(mhe::v3d(sp.pos.ll().x(), sp.pos.ll().y(), 0));
		loaded_sprite->setSize(50);	
		sprite = loaded_sprite;
	}

	boost::shared_ptr<StoneSpriteAspect> gr(
		new StoneSpriteAspect(sp.name));

	boost::shared_ptr<mhe::game::MoveAspect> move_aspect(
		new mhe::game::MoveAspect(sp.sprite_name + ".move", sprite));
	gr->attach(move_aspect);

	engine->get_aspect_manager().add(gr);
	engine->get_aspect_manager().add(move_aspect);
	 
	return gr;
}


void StoneSpriteAspect::move(const mhe::v3d& direction)
{
	DEBUG_LOG("SpriteAspect move: " << direction.x() << " " << direction.y());
	const mhe::v3d& anim_tr = direction / speed_;
	mhe::matrixf m;
	m.set_translate(anim_tr);
	mhe::game::MoveParams mp;
	mp.m = m;
	mp.upd_time = 500 / speed_;
	mp.move_time = 500;
	update_childs(mhe::game::transform_event, &mp);
}
