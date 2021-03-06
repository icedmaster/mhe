#include "game/engine.hpp"

#include "game/base_view_events_handler.hpp"
#include "utils/global_log.hpp"

#include "render/render_context.hpp"
#include "render/renderer.hpp"

#ifdef RDBG_ENABLED
#include "debug/default_rdbg_setup.hpp"
#endif

#include "debug/profiler.hpp"
#include "debug/gpu_profiler.hpp"

namespace mhe {
namespace game {

Engine::Engine()
#ifdef RDBG_ENABLED
    :
#endif
    debug_views_(*this),
    scene_(context_)
{}

bool Engine::init(uint width, uint height, uint bpp, bool fullscreen)
{
    process_ = false;

    while (true)
    {
        WindowContextFormat format;
        format.major_version = context_.driver.major_version_need();
        format.minor_version = context_.driver.minor_version_need();
        if (!context_.window_system.init(width, height, bpp, fullscreen, format))
        {
            ERROR_LOG("Can't initialize WindowSystem with format:" << format.major_version << " " <<
                      format.minor_version);

            if (!context_.driver.next_version())
            {
                const std::string& name = SystemFactory::instance().video_driver_factory().set_next_driver();
                if (name.empty())
                {
                    ASSERT(0, "All available drivers have been tried - can't initialize engine");
                    return false;
                }
                INFO_LOG("Try next driver with name:" << name);
                context_.driver.reset();
            }
            continue;
        }
        else
        {
            INFO_LOG("Initialized WindowSystem with format:" << format.major_version << " " <<
                     format.minor_version);
            break;
        }
    }

    if (!context_.driver.init())
    {
        ERROR_LOG("Driver initialization failed");
        return false;
    }
    context_.driver.set_window_size(context_.window_system.screen_size());
    INFO_LOG("Driver has been initialized");

    context_.window_system.view()->set_events_handler(new BaseViewEventsHandler(this));

#ifdef MHE_DEBUG
    context_.window_system.disable_vsync();
#endif

    context_.shader_manager.set_context(&context_);
    context_.mesh_manager.set_context(&context_);
    context_.texture_manager.set_context(&context_);
    context_.material_manager.set_context(&context_);
    context_.animation_manager.set_context(&context_);

    set_default_video_settings();

    setup_generated();

    init_standart_stat_views(stats_);

    MainGPUProfiler::create_singleton(); // GPU profiler depends on graphics context so we init it here

    stats_timer_.start();

    return true;
}

void Engine::destroy()
{
#ifdef RDBG_ENABLED
    rdbg_engine_->stop();
#endif
    renderer_->destroy();
    debug_views_.destroy();

    MainGPUProfiler::destroy_singleton();

    context_.driver.close();
    context_.window_system.close();
}

bool Engine::init_debug_components()
{
    debug_views_.init(event_manager_);
    context_.debug_views = &debug_views_;
    MaterialSystems::Values& materials = context_.material_systems.get_all_materials();
    for (size_t i = 0, size = materials.size(); i < size; ++i)
        materials[i]->init_debug_views(context_);
#ifdef RDBG_ENABLED
    rdbg_engine_.reset(new RDBGEngine(*this));
    if (!rdbg_engine_->start())
    {
        WARN_LOG("RDBG engine initialization failed");
    }
    else
    {
        INFO_LOG("RDBG engine started");
    }
    default_rdbg_setup(rdbg_engine_->processor());
#endif
    return true;
}

void Engine::set_default_video_settings()
{
    context_.driver.set_viewport(0, 0, context_.window_system.width(), context_.window_system.height());
    context_.driver.set_clear_color(color_black);
}

void Engine::reset_profiler()
{
    MainProfiler::instance().clear();
}

void Engine::run()
{
    process_ = true;
    while (process_)
    {
        MainProfiler::instance().clear();
        ProfilerElement pe("engine.run");
        update();
        render();
    }
}

void Engine::stop()
{
    process_ = false;
}

void Engine::update()
{
#ifdef RDBG_ENABLED
    rdbg_engine_->update();
#endif
    ProfilerElement pe("engine.update");

    if (global::use_vsync.reset_if_changed())
    {
        if (global::use_vsync.value())
            context_.window_system.enable_vsync();
        else context_.window_system.disable_vsync();
    }

    RenderContext& render_context = renderer_->render_context();
    render_context.tick = static_cast<uint32_t>(utils::get_current_time());
    render_context.fdelta = utils::get_last_delta();

    if (stats_timer_.elapsed() > 1.0f)
    {
        context_.driver.stats().reset();
        stats_timer_.start();
    }

    event_manager_.check(context_.window_system);

    debug_views_.update();
    update_standart_stats_views(stats_, *this);

    if (game_scene_ != nullptr)
        game_scene_->update(*this);

    scene_.update(render_context);
    renderer_->before_update(scene_.scene_context());
    scene_.process_requests(render_context);
    renderer_->update(scene_.scene_context());
}

void Engine::render()
{
    if (game_scene_ != nullptr)
        game_scene_->before_draw(*this);
    debug_views_.render();
    ProfilerElement pe("engine.render");
    renderer_->render(scene_.scene_context());

    if (game_scene_ != nullptr)
        game_scene_->draw(*this);

    renderer_->flush();
}

void init_singletons()
{
    MainProfiler::create_singleton();
    GlobalVars::create_singleton();
    MaterialSystemFactory::create_singleton();
}

void destroy_singletons()
{
    MaterialSystemFactory::destroy_singleton();
    GlobalVars::destroy_singleton();
    MainProfiler::destroy_singleton();
}

GlobalVars& get_global_vars()
{
    return GlobalVars::instance();
}

}}
