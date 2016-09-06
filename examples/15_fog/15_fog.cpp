#include "mhe.hpp"

#include <render/fog.hpp>

using namespace mhe;

#define USE_VOLUMETRIC_FOG
#define USE_VOLUMETRIC_FOG_CONFIG

#ifndef USE_VOLUMETRIC_FOG
#undef USE_VOLUMETRIC_FOG_CONFIG
#endif

class GameScene : public mhe::game::GameScene
{
public:
    bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
    {
        mhe::LightInstance& light_instance = engine.scene().create_light(Light::directional);
        mhe::Light& light = light_instance.light;
        light.shading().diffuse = mhe::color_white;
        light.shading().specular = mhe::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        light.shading().intensity = 3.0f;
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(mhe::pi_2 * 0.75f, mhe::deg_to_rad(15.0f), 0.0f));
        light.set_type(mhe::Light::directional);
        light.desc().cast_shadows = true;
        light.desc().shadowmap_bias = 0.01f;
        light_instance.enabled = true;

        mhe::NodeInstance& node = engine.scene().create_node();
        mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("sponza.mesh"), engine.context(), engine.scene_context());

        MaterialSystemContext material_system_context;
        const string fog_material_name("height_fog");
        material_system_context.priority = posteffect_material_priority_base;
        material_system_context.shader_name = string("posteffect_height_fog");
        material_system_context.instance_name = fog_material_name;
        material_system_context.material_instances_number = 0;
        engine.context().initialization_parameters.add(fog_material_name, material_system_context);

        PosteffectSystem::PosteffectNodeDesc posteffect_node_desc;
        posteffect_node_desc.name = fog_material_name;
        posteffect_node_desc.material = fog_material_name;
        posteffect_node_desc.priority = 24;
        posteffect_node_desc.instantiate = true;
        // use GBuffer's depth
        PosteffectSystem::NodeInput& input0 = posteffect_node_desc.inputs.add();
        input0.index = 0;
        input0.material = string("gbuffer_fill");
        input0.material_output = gbuffer_depth_render_target_index;
        // uniform
        PosteffectSystem::Uniforms::type& uniform0 = posteffect_node_desc.uniforms.add();
        uniform0.index = 0;
        uniform0.explicit_handle = engine.render_context().main_camera.percamera_uniform;

#ifndef USE_VOLUMETRIC_FOG
        engine.renderer()->posteffect_system().add(engine.context(), posteffect_node_desc);
        HeightFogMaterialSystem* fog_material_system = engine.context().material_systems.get<HeightFogMaterialSystem>();
        fog_material_system->settings().start = 100.0f;
        fog_material_system->settings().density = 0.5f;
        fog_material_system->settings().falloff = 0.02f;
        fog_material_system->settings().color.set(0.5f, 0.6f, 0.7f, 0.0f);

        volumetric_fog_material_system_ = nullptr;
#elif !defined(USE_VOLUMETRIC_FOG_CONFIG)

        // volumetric fog
        // step 1 - ESM
        const string esm_name("esm");
        material_system_context.shader_name = esm_name;
        material_system_context.instance_name = esm_name;
        material_system_context.options.add(string("esm_downsample_shader"), string("compute/esm_downsample"));
        material_system_context.options.add(string("esm_size"), 256);
        material_system_context.options.add(string("esm_blur_shader"), string("posteffect_blur"));
        engine.context().initialization_parameters.add(esm_name, material_system_context);
        ExponentialShadowMap* esm_material_system = create<ExponentialShadowMap>(engine.context(), esm_name, esm_name);
        ASSERT(esm_material_system != nullptr, "Couldn't create ExponentialShadowMap material system");
        engine.renderer()->set_material_system_to_process(esm_material_system);

        // step 2 - volumetric fog itself
        const string volumetric_fog_shader_name("compute/volumetric_fog");
        const string volumetric_fog_name("volumetric_fog");
        material_system_context.shader_name = volumetric_fog_shader_name;
        material_system_context.instance_name = volumetric_fog_name;
        ++material_system_context.priority;
        material_system_context.options.clear();
        material_system_context.options.add(string("volume_width"), 128);
        material_system_context.options.add(string("volume_height"), 88);
        material_system_context.options.add(string("volume_depth"), 72);
        material_system_context.options.add(string("propagation_shader"), string("compute/volumetric_fog_propagation"));
        engine.context().initialization_parameters.add(volumetric_fog_name, material_system_context);
        VolumetricFogMaterialSystem* volumetric_fog_material_system =
            create<VolumetricFogMaterialSystem>(engine.context(), volumetric_fog_name, volumetric_fog_name);
        ASSERT(volumetric_fog_material_system != nullptr, "Couldn't create VolumetricFogMaterialSystem");
        engine.renderer()->set_material_system_to_process(volumetric_fog_material_system);
        volumetric_fog_material_system->settings().density = 0.2f;
        volumetric_fog_material_system->settings().color.set(0.1f, 0.1f, 0.1f, 0.0f);
        volumetric_fog_material_system->init_debug_views(engine.context());

        // step 3 - resolve to the separate fog texture
        const string resolve_fog_name("volumetric_fog_resolve");
        const string resolve_fog_shader_name("posteffect_volumetric_fog_resolve");

        material_system_context.shader_name = resolve_fog_shader_name;
        material_system_context.instance_name = resolve_fog_name;
        material_system_context.options.clear();
        engine.context().initialization_parameters.add(resolve_fog_name, material_system_context);

        // get materials systems which will be touched by the volumetric fog material system
        TonemapMaterialSystem* tonemap_material_system = engine.context().material_systems.get<TonemapMaterialSystem>();
        ASSERT(tonemap_material_system != nullptr, "Couldn't find TonemapMaterialSystem");
        AverageLuminanceMaterialSystem* average_luminance_material_system = 
            engine.context().material_systems.get<AverageLuminanceMaterialSystem>();
        ASSERT(average_luminance_material_system != nullptr, "Couldn't find AverageLuminanceMaterialSystem");
        BloomMaterialSystem* bloom_material_system = 
            engine.context().material_systems.get<BloomMaterialSystem>();
        ASSERT(bloom_material_system != nullptr, "Couldn't find BloomMaterialSystem");

        posteffect_node_desc.name = resolve_fog_name;
        posteffect_node_desc.material = resolve_fog_name;
        posteffect_node_desc.priority = 2;
        posteffect_node_desc.instantiate = true;

        {
            posteffect_node_desc.inputs.clear();
            posteffect_node_desc.outputs.clear();
            // use tonemapper's input as fog resolving input
            PosteffectSystem::NodeInput& input0 = posteffect_node_desc.inputs.add();
            input0.index = 0;
            input0.explicit_texture = tonemap_material_system->input(0);
            // use GBuffer's depth
            PosteffectSystem::NodeInput& input1 = posteffect_node_desc.inputs.add();
            input1.index = 3;
            input1.material = string("gbuffer_fill");
            input1.material_output = gbuffer_depth_render_target_index;
            // fog texture
            PosteffectSystem::NodeInput& input2 = posteffect_node_desc.inputs.add();
            input2.index = 4;
            input2.material = volumetric_fog_name;
            input2.material_output = 0;
            // uniform
            posteffect_node_desc.uniforms.clear();
            PosteffectSystem::Uniforms::type& uniform0 = posteffect_node_desc.uniforms.add();
            uniform0.index = 0;
            uniform0.explicit_handle = engine.render_context().main_camera.percamera_uniform;
            PosteffectSystem::Uniforms::type& uniform1 = posteffect_node_desc.uniforms.add();
            uniform1.index = 1;
            uniform1.explicit_handle = volumetric_fog_material_system->settings_uniform_id();
            // output
            PosteffectSystem::NodeOutput& output = posteffect_node_desc.outputs.add();
            output.index = 0;
            output.scale = 1.0f;
            output.format = format_rgb16f;
        }
        VolumetricFogResolveMaterialSystem* volumetric_fog_resolve_material_system = 
            static_cast<VolumetricFogResolveMaterialSystem*>(engine.renderer()->posteffect_system().add(engine.context(), posteffect_node_desc));

        average_luminance_material_system->set_input(0, volumetric_fog_resolve_material_system->output(0));
        bloom_material_system->set_input(0, volumetric_fog_resolve_material_system->output(0));
        tonemap_material_system->set_input(0, volumetric_fog_resolve_material_system->output(0));

        volumetric_fog_material_system_ = volumetric_fog_material_system;
        esm_material_system_ = esm_material_system;
#endif

        return true;
    }

    bool update(mhe::game::Engine& engine) override
    {
        return true;
    }

    void before_draw(game::Engine& engine) override
    {
#ifndef USE_VOLUMETRIC_FOG_CONFIG
        if (volumetric_fog_material_system_ != nullptr)
            volumetric_fog_material_system_->set_light_instance(esm_material_system_->light_instance_id(),
                esm_material_system_->shadowmap_texture(), esm_material_system_->settings_uniform_id());
#endif
    }

    VolumetricFogMaterialSystem* volumetric_fog_material_system_;
    ExponentialShadowMap* esm_material_system_;
};

int main(int /*argc*/, char** /*argv*/)
{
    mhe::app::Application app("15_fog");
    mhe::app::ApplicationConfig config;
    config.width = 1280;
    config.height = 720;
    config.bpp = 32;
    config.fullscreen = false;
#ifndef MHE_VS
    config.assets_path = "../assets/";
#else
    config.assets_path = "../../assets/";
#endif

#ifndef USE_VOLUMETRIC_FOG_CONFIG
    config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_without_postprocess.xml");
#else
    config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_volumetric_fog.xml");
#endif
    app.init(config);

    mhe::game::GameSceneDesc desc;
    GameScene* game_scene = new GameScene;
    game_scene->init(app.engine(), desc);
    app.engine().set_game_scene(game_scene);

    mhe::PerspectiveCameraParameters camera_parameters;
    camera_parameters.fov = 60.0f;
    camera_parameters.znear = 0.5f;
    camera_parameters.zfar = 3000.0f;
    mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
        mhe::vec3(0, 1, 5), mhe::vec3(0.0f, mhe::pi, 0));
    camera_controller->set_move_speed(150.0f);
    camera_controller->set_rotation_speed(50.0f);
    app.engine().scene().set_camera_controller(camera_controller);

    return app.run();
}
