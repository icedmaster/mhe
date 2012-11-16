#include "app/application.hpp"

#include "utils/sysutils.hpp"

namespace mhe {

Application::Application()
{}

Application::Application(const ArgumentsParser& /*arguments_parser*/)
{
}

bool Application::init(const std::string& name, const Config& config)
{
	name_ = name;
	if (!mhe_app_init(config)) return false;
	init_impl();
	return true;
}

void Application::deinit()
{
	deinit_impl();
	mhe_app_deinit();
}

bool Application::mhe_app_init(const Config& config)
{
	mhe::utils::create_standart_log();
	mhe::utils::init_randomizer();
	mhe::impl::start_platform();
	bool result = engine_.init(config.width, config.height, config.bpp, config.fullscreen);
	if (result)
		engine_.context().window_system().set_caption(name_);
	return result;
}

void Application::mhe_app_deinit()
{
	mhe::impl::stop_platform();
}	

int Application::run_impl(const boost::shared_ptr<game::GameScene>& first_scene)
{
	engine_.set_game_scene(first_scene);
	engine_.run();
	deinit();
	return 0;
}

}
