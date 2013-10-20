#ifndef __APPLICATION_HPP__ 
#define __APPLICATION_HPP__

#include "application_config.hpp"
#include "arguments_parser.hpp"
#include "game/engine.hpp"

namespace mhe {
namespace app {

class Application
{
public:
	Application(const std::string& name = "");
	Application(const ArgumentsParser& arg_parser);
	virtual ~Application() {}
	bool init(const ApplicationConfig& config);
	void deinit();
	int run(const boost::shared_ptr<game::GameScene>& first_scene)
	{		
		engine_.set_game_scene(first_scene); // TODO: maybe, need to move it into run_impl() ?
		return run_impl();
	}

	int run(game::GameScene* first_scene)
	{
		return run(boost::shared_ptr<game::GameScene>(first_scene));
	}

	int run()
	{
		return run_impl();
	}

	const std::string& name() const
	{
		return name_;
	}

	const game::Engine& engine() const
	{
		return engine_;
	}

	game::Engine& engine()
	{
		return engine_;
	}

	std::string path() const
	{
		return application_base_path();
	}
protected:
	// methods with default implementation
	virtual bool mhe_app_init(const ApplicationConfig& config);
	virtual void mhe_app_deinit();
	virtual int run_impl();
	virtual void init_assets_path(const std::string& config_assets_path);

	virtual std::string application_base_path() const
	{
		return "";
	}
private:
	virtual void init_impl() {}
	virtual void deinit_impl() {}
	std::string name_;
	game::Engine engine_;
};

}}

#endif
