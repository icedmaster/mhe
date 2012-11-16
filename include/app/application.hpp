#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "application_config.hpp"
#include "arguments_parser.hpp"
#include "game/engine.hpp"

namespace mhe {

class Application
{
public:
	Application();
	Application(const ArgumentsParser& arg_parser);
	virtual ~Application() {}
	bool init(const std::string& name, const ApplicationConfig& config);
	void deinit();
	int run(const boost::shared_ptr<game::GameScene>& first_scene)
	{
		return run_impl(first_scene);
	}

	int run(game::GameScene* first_scene)
	{
		return run(boost::shared_ptr<game::GameScene>(first_scene));
	}

	const std::string& name() const
	{
		return name_;
	}

	const game::Engine& engine() const
	{
		return engine_;
	}
protected:
	// methods with default implementation
	virtual bool mhe_app_init(const ApplicationConfig& config);
	virtual void mhe_app_deinit();
	virtual int run_impl(const boost::shared_ptr<game::GameScene>& first_scene);

	game::Engine& engine()
	{
		return engine_;
	}
private:
	virtual void init_impl() {}
	virtual void deinit_impl() {}

	std::string name_;
	game::Engine engine_;
};

}

#endif
