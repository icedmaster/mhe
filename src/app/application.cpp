#include "app/application.hpp"

#include "utils/sysutils.hpp"
#include "utils/file_utils.hpp"
#include "app/application_asset_path.hpp"
#include "core/memory.hpp"
#include "events/delegate_event_listener.hpp"
#include "events/system_event.hpp"
#include "events/system_device.hpp"

namespace mhe {
namespace app {

Application::Application(const std::string& name) :
	name_(name)
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

void Application::close()
{
	close_impl();
	mhe_app_close();
}

bool Application::mhe_app_init(const ApplicationConfig& config)
{
	mhe::utils::init_randomizer();
	mhe::impl::start_platform();
	INFO_LOG("Init application with name:" << name_ << " w:" << config.width <<
			 " h:" << config.height << " bpp:" << config.bpp);
	init_assets_path(config.assets_path);
	init_default_assets(config);
  if (engine_.init(config.width, config.height, config.bpp, config.fullscreen))
  {
      add_delegates();
      return true;
  }
  return false;
}

void Application::mhe_app_close()
{
	mhe::impl::stop_platform();
}	

int Application::run_impl()
{
	MainLoop* main_loop = engine_.context().window_system.main_loop();
	if (main_loop == nullptr)
		engine_.run();
	else main_loop->run(1000 / 60.0f);
	close();
	return 0;
}

void Application::stop_impl()
{
	MainLoop* main_loop = engine_.context().window_system.main_loop();
	if (main_loop == nullptr)
		engine_.stop();
	else main_loop->stop();
}

void Application::init_assets_path(const std::string& config_assets_path)
{	
	std::string base_path = utils::path_join(application_base_path(),
		config_assets_path.empty() ? assets_base_path : config_assets_path);
    INFO_LOG("Application::init_assets_path with base path:" << base_path);
}

void Application::init_default_assets(const ApplicationConfig& config)
{
}

void Application::add_delegates()
{
    class ApplicationEventListener : public DelegateEventListener
    {
    public:
        ApplicationEventListener(Application* app) :
            DelegateEventListener(system_event_type, 0, 0, create_delegate(app, &Application::on_system_event))
        {}
    };

		engine_.event_manager().add_device(new SystemDevice("sys"));
    engine_.event_manager().add_listener(new ApplicationEventListener(this));
}

bool Application::on_system_event(const Event* event)
{
    const SystemEvent* se = static_cast<const SystemEvent*>(event);
    if (se->arg() == SystemEvent::quit)
        stop();
    return true;
}

}}
