#ifndef __DEMO_GAME_SCENE_HPP__
#define __DEMO_GAME_SCENE_HPP__

#include "mhe.hpp"
#include "game_stone.hpp"
#include "field.hpp"

class DemoGameScene : public mhe::game::GameScene
{
public:
	DemoGameScene(mhe::game::Engine* engine) : mhe::game::GameScene(engine) {}
private:
	bool init_impl(const std::string& arg, void*)
	{
		load_scene(arg);
		return true;
	}

	void load_scene(const std::string& arg)
	{
		mhe::game::mhe_loader loader(arg, get_engine());
		loader.load_all_assets();
		fps_font = get_engine()->getFontManager().get("145");
		fps_font->setColor(mhe::cfWhite);
		boost::shared_ptr<mhe::iNode> fps_node(
			new mhe::utils::GraphicsFPSCounter(get_engine()->getInputSystem(),
											   fps_font, mhe::v2d(20, 550)));
		get_scene()->add(fps_node);
		create_field(loader, arg);
		get_engine()->getInputSystem()->enable_input();
	}

	void create_field(const mhe::game::mhe_loader& loader, const std::string& arg)
	{
		std::vector<int> row(3);
		std::vector< std::vector<int> > stones;
		row[0] = 0; row[1] = 1; row[2] = 1;
		stones.push_back(row);
		row[0] = 1; row[1] = 0; row[2] = 1;
		stones.push_back(row);
		GameField game_field(loader, mhe::rect<int>(200, 200, 150, 100), stones);
	}
private:
	boost::shared_ptr<mhe::gui::iFont> fps_font;	
};

#endif
