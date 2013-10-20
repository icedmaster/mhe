#ifndef _GAME_SCENE_HPP_
#define _GAME_SCENE_HPP_

#include "scene.hpp"
#include "utils/properties_list.hpp"

namespace mhe {
namespace game {
class Engine;

// base game scene class

class GameScene
{
public:
	GameScene(Engine* engine) :
		engine_(engine),
		scene_(new Scene)
	{}

	virtual ~GameScene() {}
				
	bool init(const mhe::utils::PropertiesList& plist = mhe::utils::PropertiesList::empty())
	{
		return init_impl(plist);
	}

	void deinit();
				
	void set_next_scene(boost::shared_ptr<GameScene> gs)
	{
		next_ = gs;
	}
				
	boost::shared_ptr<GameScene> get_next_scene() const
	{
		return next_;
	}
				
	void set_prev_scene(boost::shared_ptr<GameScene> gs)
	{
		prev_ = gs;
	}
				
	boost::shared_ptr<GameScene> get_prev_scene() const
	{
		return prev_;
	}

	void set_scene(boost::shared_ptr<Scene> scene)
	{
		scene_ = scene;
	}

	boost::shared_ptr<Scene> scene() const
	{
		return scene_;
	}

	bool process()
	{
		return process_impl();
	}

	void draw()
	{
		draw_impl();
	}

	Engine* engine() const
	{
		return engine_;
	}
protected:
	Engine* get_engine() const
	{
		return engine_;
	}

	virtual void free_resources();
private:
	virtual bool init_impl(const mhe::utils::PropertiesList& /*plist*/) {return true;}
	virtual void deinit_impl() {}
	virtual bool process_impl() {return true;}
	virtual void draw_impl() {}
private:
	Engine* engine_;
	boost::shared_ptr<GameScene> prev_;
	boost::shared_ptr<GameScene> next_;
				
	boost::shared_ptr<Scene> scene_;
};

}}

#endif
