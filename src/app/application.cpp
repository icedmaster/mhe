#include "app/application.hpp"

#include "utils/sysutils.hpp"
#include "utils/file_utils.hpp"
#include "app/application_asset_path.hpp"
#include "core/memory.hpp"
#include "core/string.hpp"
#include "events/delegate_event_listener.hpp"
#include "events/system_event.hpp"
#include "events/system_device.hpp"
#include "events/keyboard_device.hpp"
#include "events/mouse_device.hpp"

// TODO: move render initialization out of this file
#include "render/deferred_renderer.hpp"
#include "render/posteffect_material_system.hpp"

namespace mhe {
namespace app {

struct PosteffectParams
{
    fixed_size_vector<PosteffectSystem::PosteffectNodeDesc, 16> nodes;
};

struct RendererParams
{
    string skybox;
    string shadowmap_depth_write;
    string directional_depth_write;
    string fullscreen_debug;
    string probes_accumulator;
    PosteffectParams posteffect_params;
};

Application::Application(const char* name) :
    name_(name)
{
#ifndef MHE_STANDART_LOG_DISABLED
    mhe::utils::create_standart_log();
#endif
    create_default_allocator();
    game::init_singletons();
    engine_ = new game::Engine;
}

Application::~Application()
{
    destroy_render(engine_->context());
    engine_->destroy();
    delete engine_;
    game::destroy_singletons();

    destroy_default_allocator();
    print_memory_info();

#ifndef MHE_STANDART_LOG_DISABLED
    mhe::utils::destroy_standart_log();
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
    if (engine_->init(config.width, config.height, config.bpp, config.fullscreen))
    {
        add_delegates();
        init_render(config);
        engine_->init_debug_components();
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
    MainLoop* main_loop = engine_->context().window_system.main_loop();
    if (main_loop == nullptr)
        engine_->run();
    else main_loop->run(static_cast<unsigned int>(1000 / 60.0f));
    close();
    return 0;
}

void Application::stop_impl()
{
    MainLoop* main_loop = engine_->context().window_system.main_loop();
    if (main_loop == nullptr)
        engine_->stop();
    else main_loop->stop();
}

void Application::init_assets_path(const std::string& config_assets_path)
{   
    std::string base_path = utils::path_join(application_base_path(),
        config_assets_path.empty() ? assets_base_path : config_assets_path);
    INFO_LOG("Application::init_assets_path with base path:" << base_path);

    engine_->context().shader_manager.set_path(utils::path_join(base_path, shader_path));
    engine_->context().mesh_manager.set_path(utils::path_join(base_path, mesh_path));
    engine_->context().texture_manager.set_path(utils::path_join(base_path, texture_path));
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

    engine_->event_manager().add_device(new SystemDevice("sys"));
    engine_->event_manager().add_keyboard(new KeyboardDevice("kbd"));
    engine_->event_manager().add_mouse(new MouseDevice("mouse"));
    engine_->event_manager().add_listener(new ApplicationEventListener(this));
}

bool Application::on_system_event(const Event* event)
{
    const SystemEvent* se = static_cast<const SystemEvent*>(event);
    if (se->arg() == SystemEvent::quit)
        stop();
    return true;
}

void Application::init_render(const ApplicationConfig& config)
{
    mhe::init_render(engine_->context());

    std::fstream f(config.render_config_filename.c_str(), std::ios::in | std::ios::binary);
    if (!f)
    {
        WARN_LOG("Can't open render config with filename " << config.render_config_filename);
        return;
    }

    pugi::xml_document document;
    if (!document.load(f))
        return;

    pugi::xml_node root = document.root();
    if (!root) return;
    pugi::xml_node mhe_node = root.child("mhe");
    if (!mhe_node) return;
    pugi::xml_node materials_node = mhe_node.child("materials");
    if (!materials_node) return;

    init_materials(materials_node);

    RendererParams renderer_params;
    pugi::xml_node skybox_node = mhe_node.child("skybox");
    if (skybox_node) renderer_params.skybox = skybox_node.attribute("name").value();
    pugi::xml_node shadowmap_depth_write_node = mhe_node.child("shadowmap_depth_write");
    if (shadowmap_depth_write_node) renderer_params.shadowmap_depth_write = shadowmap_depth_write_node.attribute("name").value();
    pugi::xml_node directional_shadowmap_depth_write_node = mhe_node.child("directional_shadowmap_depth_write");
    if (directional_shadowmap_depth_write_node)
        renderer_params.directional_depth_write = directional_shadowmap_depth_write_node.attribute("name").value();
    pugi::xml_node fullscreen_debug_node = mhe_node.child("fullscreen_debug");
    if (fullscreen_debug_node) renderer_params.fullscreen_debug = fullscreen_debug_node.attribute("name").value();
    pugi::xml_node probes_accumulator_node = mhe_node.child("probes_accumulator");
    if (probes_accumulator_node) renderer_params.probes_accumulator = probes_accumulator_node.attribute("name").value();

    init_posteffect_parameters(mhe_node.child("posteffects"), renderer_params);
    
    pugi::xml_node gbuffer_node = mhe_node.child("gbuffer");
    if (gbuffer_node)
        init_gbuffer(gbuffer_node, renderer_params);
    else
        engine_->set_renderer(new NullRenderer(engine_->context()));
    init_posteffects(renderer_params);
}

void Application::init_posteffect_parameters(pugi::xml_node node, RendererParams& params) const
{
    if (!node) return;
    for (pugi::xml_node n = node.child("node"); n; n = n.next_sibling("node"))
    {
        const char* material_name = n.attribute("material").value();
        const char* name = n.attribute("name").value();
        const char* prev_node_name = n.attribute("src").value();
        size_t priority = n.attribute("priority").as_uint();
        bool instantiate = n.attribute("instantiate").as_bool();
        PosteffectSystem::PosteffectNodeDesc posteffect_node;
        posteffect_node.name = name;
        posteffect_node.prev_node = prev_node_name;
        posteffect_node.material = material_name;
        posteffect_node.priority = priority;
        posteffect_node.instantiate = instantiate;

        pugi::xml_node inputs_node = n.child("inputs");
        if (inputs_node)
        {
            for (pugi::xml_node i = inputs_node.child("input"); i; i = i.next_sibling("input"))
            {
                PosteffectSystem::NodeInput& input = posteffect_node.inputs.add();
                pugi::xml_attribute attr = i.attribute("index");
                ASSERT(attr, "Malformed posteffect node: input index attribute must exists");
                input.index = attr.as_uint();
                attr = i.attribute("node");
                if (attr) input.node = attr.value();
                attr = i.attribute("output");
                if (attr) input.node_output = attr.as_uint();
                attr = i.attribute("material");
                if (attr) input.material = attr.value();
                attr = i.attribute("material_output");
                if (attr) input.material_output = attr.as_uint();
                attr = i.attribute("copy_current_framebuffer");
                if (attr) input.copy_current_framebuffer = attr.as_bool();
                else input.copy_current_framebuffer = false;
            }
        }

        pugi::xml_node outputs_node = n.child("outputs");
        if (outputs_node)
        {
            for (pugi::xml_node o = outputs_node.child("output"); o; o = o.next_sibling("output"))
            {
                PosteffectSystem::NodeOutput& output = posteffect_node.outputs.add();
                pugi::xml_attribute attr = o.attribute("index");
                ASSERT(attr, "Malformed posteffect node: output index attribute must exist");
                output.index = attr.as_uint();
                attr = o.attribute("scale");
                if (attr) output.scale = attr.as_float();
                else output.scale = 1.0f;
                attr = o.attribute("format");
                if (attr) output.format = get_format_by_name(attr.value());
                else output.format = format_max;
            }
        }

        pugi::xml_node buffers_node = n.child("buffers");
        if (buffers_node)
        {
            for (pugi::xml_node b = buffers_node.child("buffer"); b; b = b.next_sibling("buffer"))
            {
                PosteffectSystem::Buffer& buffer = posteffect_node.buffers.add();
                pugi::xml_attribute attr = b.attribute("index");
                ASSERT(attr, "Malformed posteffect node. The index of a buffer must exist");
                buffer.index = attr.as_uint();
                attr = b.attribute("node");
                ASSERT(attr, "Malformed posteffect node. The input node of a buffer must exist");
                buffer.node = attr.value();
                attr = b.attribute("buffer");
                ASSERT(attr, "Malformed posteffect node. The buffer index of input node for a buffer must exist");
                buffer.node_buffer = attr.as_uint();
            }
        }

        pugi::xml_node uniforms_node = n.child("uniforms");
        if (uniforms_node)
        {
            for (pugi::xml_node u = uniforms_node.child("uniform"); u; u = u.next_sibling("uniform"))
            {
                PosteffectSystem::Buffer& buffer = posteffect_node.uniforms.add();
                pugi::xml_attribute attr = u.attribute("index");
                ASSERT(attr, "Malformed posteffect node. The index of a uniform must exist");
                buffer.index = attr.as_uint();
                attr = u.attribute("node");
                ASSERT(attr, "Malformed posteffect node. The input node of a uniform must exist");
                buffer.node = attr.value();
                attr = u.attribute("uniform");
                ASSERT(attr, "Malformed posteffect node. The uniform index of input node for a uniform must exist");
                buffer.node_buffer = attr.as_uint();
            }
        }

        params.posteffect_params.nodes.push_back(posteffect_node);
    }
}

void Application::init_materials(pugi::xml_node materials_node)
{
    Context& context = engine_->context();
    
    MaterialSystemContext material_context;
    for (pugi::xml_node n = materials_node.child("material"); n; n = n.next_sibling("material"))
    {
        const char* name = n.attribute("name").value();
        string material_instance_name = n.attribute("instance_name").value();
        const char* shader_name = n.attribute("shader").value();
        const char* defs = n.attribute("defs").value();
        unsigned int priority = n.attribute("priority").as_uint();
        unsigned int material_instances_number = n.attribute("instances").as_uint();
        bool on_demand = n.attribute("on_demand").as_bool();

        material_context.options.clear();
        if (pugi::xml_node options_node = n.child("options"))
        {
            for (pugi::xml_node o = options_node.first_child(); o; o = o.next_sibling())
                material_context.options.add(o.name(), o.child_value());
        }

        if (material_instance_name.empty())
            material_instance_name = name;

        material_context.shader_name = shader_name;
        material_context.instance_name = material_instance_name;
        material_context.defs[0] = defs;
        material_context.priority = priority;
        material_context.material_instances_number = material_instances_number;

        context.initialization_parameters.add(name, material_context);

        if (!on_demand)
        {
            MaterialSystem* material_system = create(context, name, material_instance_name);
            if (material_system == nullptr)
            {
                WARN_LOG("MaterialSystem initialization failed:" << name);
                continue;
            }
        }
    }
}

void Application::init_gbuffer(pugi::xml_node gbuffer_node, const RendererParams& params)
{
    Context& context = engine_->context();

    pugi::xml_node fill_node = gbuffer_node.child("fill");
    pugi::xml_node use_node = gbuffer_node.child("use");
    pugi::xml_node draw_node = gbuffer_node.child("draw");
    const char* fill_name = fill_node.attribute("name").value();
    const char* use_name = use_node.attribute("name").value();
    const char* draw_name = draw_node.attribute("name").value();
    AbstractGBufferFillMaterialSystem* fill_material_system = context.material_systems.get<AbstractGBufferFillMaterialSystem>(fill_name);
    ASSERT(fill_material_system != nullptr, "Unable to initialize material system with name:" << fill_name);
    AbstractGBufferUseMaterialSystem* use_material_system = context.material_systems.get<AbstractGBufferUseMaterialSystem>(use_name);
    PosteffectMaterialSystemBase* draw_material_system = context.material_systems.get<PosteffectMaterialSystemBase>(draw_name);

    MaterialSystem* skybox_material_system = context.material_systems.get(params.skybox);
    MaterialSystem* depth_write_material_system = context.material_systems.get(params.shadowmap_depth_write);
    MaterialSystem* directional_depth_write_material_system = context.material_systems.get(params.directional_depth_write);
    PosteffectDebugMaterialSystem* fullscreen_debug_material_system = context.material_systems.get<PosteffectDebugMaterialSystem>(params.fullscreen_debug);
    ProbesAccumulatorMaterialSystem* probes_accumulator_material_system = context.material_systems.get<ProbesAccumulatorMaterialSystem>(params.probes_accumulator);
    
    DeferredRenderer* renderer = new DeferredRenderer(context);
    renderer->set_skybox_material_system(skybox_material_system);
    renderer->set_shadowmap_depth_write_material_system(depth_write_material_system);
    renderer->set_directional_shadowmap_depth_write_material_system(directional_depth_write_material_system);
    renderer->set_fullscreen_debug_material_system(fullscreen_debug_material_system);
    renderer->set_probes_accumulator_material_system(probes_accumulator_material_system);
    renderer->init(fill_material_system, use_material_system, draw_material_system);
    engine_->set_renderer(renderer);
}

void Application::init_posteffects(const RendererParams& params)
{
    Context& context = engine_->context();
    PosteffectSystem& posteffect_system = engine_->renderer()->posteffect_system();
    for (size_t i = 0, size = params.posteffect_params.nodes.size(); i < size; ++i)
    {
        const PosteffectSystem::PosteffectNodeDesc& node = params.posteffect_params.nodes[i];
        posteffect_system.add(context, node);
    }
}

}}
