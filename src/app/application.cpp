#include "app/application.hpp"

#include "utils/sysutils.hpp"
#include "app/application_asset_path.hpp"
#include "app/base_view_events_handler.hpp"

namespace mhe {
namespace app {

Application::Application(const std::string& name) :
	name_(name)
{
#ifndef MHE_STANDART_LOG_DISABLED
	mhe::utils::create_standart_log();
#endif
}

Application::Application(const ArgumentsParser& /*arguments_parser*/)
{
#ifndef MHE_STANDART_LOG_DISABLED
	mhe::utils::create_standart_log();
#endif
}

bool Application::init(const ApplicationConfig& config)
{
	if (!mhe_app_init(config)) return false;
	init_impl();
	return true;
}

void Application::deinit()
{
	deinit_impl();
	mhe_app_deinit();
}

bool Application::mhe_app_init(const ApplicationConfig& config)
{
	mhe::utils::init_randomizer();
	mhe::impl::start_platform();
	INFO_LOG("Init application with name:" << name_ << " w:" << config.width <<
			 " h:" << config.height << " bpp:" << config.bpp);
	bool result = engine_.init(config.width, config.height, config.bpp, config.fullscreen);
	if (!result) return false;
	init_assets_path();
	engine_.context().window_system().set_caption(name_);	
	engine_.context().window_system().view()->set_events_handler(new BaseViewEventsHandler(&engine_));
	return result;
}

void Application::mhe_app_deinit()
{
	mhe::impl::stop_platform();
}	

int Application::run_impl()
{
	engine_.run();
	deinit();
	return 0;
}

void Application::init_assets_path()
{
	std::string base_path = utils::path_join(application_base_path(), assets_base_path);
    INFO_LOG("Application::init_assets_path with base path:" << base_path);
	engine_.context().texture_manager().set_path(utils::path_join(base_path,
																  texture_path));
	engine_.font_manager().set_path(utils::path_join(base_path,
													 font_path));
}

}}
