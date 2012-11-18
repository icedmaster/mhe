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
	Application(const std::string& name);
	Application(const ArgumentsParser& arg_parser);
	virtual ~Application() {}
	bool init(const ApplicationConfig& config,
			  const boost::shared_ptr<game::GameScene>& first_scene);
	bool init(const ApplicationConfig& config, game::GameScene* first_scene)
	{
		return init(config, boost::shared_ptr<game::GameScene>(first_scene));
	}

	void deinit();
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
protected:
	// methods with default implementation
	virtual bool mhe_app_init(const ApplicationConfig& config,
							  const boost::shared_ptr<game::GameScene>& first_scene);
	virtual void mhe_app_deinit();
	virtual int run_impl();

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

}}

#endif
