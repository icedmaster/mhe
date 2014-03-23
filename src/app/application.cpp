#include "app/application.hpp"

#include "utils/sysutils.hpp"
#include "app/application_asset_path.hpp"
#include "memory.hpp"

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
	init_assets_path(config.assets_path);
	init_default_assets(config);
	engine_.context().window_system().set_caption(name_);	
	return result;
}

void Application::mhe_app_deinit()
{
	mhe::impl::stop_platform();
}	

int Application::run_impl()
{
	MainLoop* main_loop = engine_.context().window_system().main_loop();
	if (main_loop == nullptr)
		engine_.run();
	else main_loop->run(1000 / 60);
	deinit();
	return 0;
}

void Application::init_assets_path(const std::string& config_assets_path)
{	
	std::string base_path = utils::path_join(application_base_path(),
		config_assets_path.empty() ? assets_base_path : config_assets_path);
    INFO_LOG("Application::init_assets_path with base path:" << base_path);
	engine_.context().texture_manager().set_path(utils::path_join(base_path,
																  texture_path));
	engine_.font_manager().set_path(utils::path_join(base_path,
													 font_path));
	engine_.context().shader_manager().set_path(utils::path_join(base_path,
		shader_path));
}

void Application::init_default_assets(const ApplicationConfig& config)
{
	if (!engine_.context().texture_manager().load(config.default_texture_name))
	{
		Texture* texture = SystemFactory::instance().create_texture();
		texture->set_color(color_white);
		engine_.context().texture_manager().add_default(texture);
	}

	// shaders should be loaded from file
	const std::string& shader_name = (config.default_shader_name.empty()) ?
		default_shader_name : config.default_shader_name;
	shader_program_ptr shader = engine_.context().shader_manager().get(shader_name);
	if (shader != nullptr)
		engine_.context().shader_manager().add_default(shader);

	Material* material = new Material(engine_.context().texture_manager().get_default(),
									  engine_.context().shader_manager().get_default());
	engine_.context().material_manager().add_default(material);
}

}}
