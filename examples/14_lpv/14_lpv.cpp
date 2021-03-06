#include "mhe.hpp"
#include "render/rsm_material_system.hpp"
#include "render/lpv_material_system.hpp"

//#define SPONZA

#ifndef SPONZA
const char* mesh_name = "test-scene-simple.mesh";
//const char* mesh_name = "lighting-test-simple.bin";
#else
const char* mesh_name = "sponza.bin";
#endif

using namespace mhe;

class GameScene : public mhe::game::GameScene
{
public:
    bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
    {
        debug_mode_on_ = false;

        mhe::LightInstance& light_instance = engine.scene().create_light(mhe::directional);
        mhe::res::Light& light = light_instance.dblight;
        light.diffuse_color = mhe::color_rgb_white;
        light.specular_color = mhe::color_rgb_white;
        light.intensity = 3.0f;
#ifndef SPONZA
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(0.0f, -mhe::pi_2 * 1.3f, mhe::pi_2 * 0.7f));
#else
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(mhe::pi_2 * 0.75f, mhe::deg_to_rad(15.0f), 0.0f));
#endif
        light.cast_shadows = true;
        light.shadowmap_bias = 0.01f;
        light_instance.enabled = true;

        mhe::NodeInstance& node = engine.scene().create_node();
        mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string(mesh_name), engine.context(), engine.scene_context());

        // This is a small hack, because I don't include the skybox contribution during the average luminance calculation
        // and the results on the test scene where skybox may cover a big part of screen may be incorrect
        AverageLuminanceMaterialSystem* average_luminance_material_system = engine.context().material_systems.get<AverageLuminanceMaterialSystem>();
        average_luminance_material_system->settings().mode = AverageLuminanceMaterialSystem::fixed_luminance_mode;

        const string rsm_name("rsm");
        MaterialSystemContext material_system_context;
        material_system_context.priority = DeferredRenderer::deferred_renderer_base_priority - 2;
        material_system_context.shader_name = rsm_name;
        material_system_context.instance_name = rsm_name;
        material_system_context.material_instances_number = 0;
        engine.context().initialization_parameters.add(rsm_name, material_system_context);

        rsm_material_system_ = static_cast<RSMMaterialSystem*>(create_material_system(engine.context(), rsm_name, rsm_name));
        engine.renderer()->set_material_system_to_process(rsm_material_system_);
        rsm_material_system_->init_debug_views(engine.context());
        rsm_material_system_->settings().mode = RSMMaterialSystem::mode_volume;
        rsm_material_system_->settings().volume_aabb = AABBf(vec3(0.0f, 2.0f, 0.0f), vec3(6.0f, 2.49f, 6.0f));

        const string lpv_name("lpv");
        const string propagation_shader_name("lpv_propagation");
        const string geometry_injection_shader_name("lpv_geometry_injection");
        material_system_context.priority = DeferredRenderer::deferred_renderer_base_priority - 1;
        material_system_context.shader_name = lpv_name;
        material_system_context.instance_name = lpv_name;
        material_system_context.material_instances_number = 0;
        material_system_context.options.add(string("injection_shader"), lpv_name);
        material_system_context.options.add(string("propagation_shader"), propagation_shader_name);
        material_system_context.options.add(string("geometry_injection_shader"), geometry_injection_shader_name);
        engine.context().initialization_parameters.add(lpv_name, material_system_context);

        lpv_material_system_ = static_cast<LPVMaterialSystem*>(create_material_system(engine.context(), lpv_name, lpv_name));
        engine.renderer()->set_material_system_to_process(lpv_material_system_);
        lpv_material_system_->set_gbuffer(rsm_material_system_->gbuffer());
        lpv_material_system_->init_debug_views(engine.context());
        lpv_material_system_->settings().mode = LPVMaterialSystem::mode_rsm;

        const string lpv_resolve_name("lpv_resolve");
        material_system_context.priority = posteffect_material_priority_base;
        material_system_context.shader_name = lpv_resolve_name;
        material_system_context.instance_name = lpv_resolve_name;
        material_system_context.material_instances_number = 0;
        engine.context().initialization_parameters.add(lpv_resolve_name, material_system_context);

        PosteffectSystem::PosteffectNodeDesc posteffect_node_desc;
        posteffect_node_desc.name = lpv_resolve_name;
        posteffect_node_desc.material = lpv_resolve_name;
        posteffect_node_desc.priority = 0;
        posteffect_node_desc.instantiate = true;
        // use GBuffer's depth and normal...
        PosteffectSystem::NodeInput& input0 = posteffect_node_desc.inputs.add();
        input0.index = 0;
        input0.material = string("gbuffer_fill");
        input0.material_output = 1;
        PosteffectSystem::NodeInput& input1 = posteffect_node_desc.inputs.add();
        input1.index = 1;
        input1.material = string("gbuffer_fill");
        input1.material_output = gbuffer_depth_render_target_index;
        // ...and 3 channels of LPV RT
        PosteffectSystem::NodeInput& input2 = posteffect_node_desc.inputs.add();
        input2.index = 2;
        input2.material = lpv_name;
        input2.material_output = 0;
        PosteffectSystem::NodeInput& input3 = posteffect_node_desc.inputs.add();
        input3.index = 3;
        input3.material = lpv_name;
        input3.material_output = 1;
        PosteffectSystem::NodeInput& input4 = posteffect_node_desc.inputs.add();
        input4.index = 4;
        input4.material = lpv_name;
        input4.material_output = 2;
        // resolve-to buffer
        PosteffectSystem::NodeOutput& output0 = posteffect_node_desc.outputs.add();
        output0.format = format_rgb16f;
        output0.index = 0;
        output0.scale = 1.0f;
        // we also need a couple of uniforms
        PosteffectSystem::Uniforms::type& uniform0 = posteffect_node_desc.uniforms.add();
        uniform0.index = 0;
        uniform0.explicit_handle = engine.render_context().main_camera.percamera_uniform;
        PosteffectSystem::Uniforms::type& uniform1 = posteffect_node_desc.uniforms.add();
        uniform1.index = 1;
        uniform1.explicit_handle = lpv_material_system_->injection_settings_uniform();

        lpv_resolve_material_system_ = static_cast<LPVResolveMaterialSystem*>(
            engine.renderer()->posteffect_system().create(engine.context(), posteffect_node_desc));
        lpv_resolve_material_system_->set_priority(DeferredRenderer::deferred_renderer_gbuffer_modifier_priority - 1);
        engine.renderer()->set_material_system_to_process(lpv_resolve_material_system_);
        engine.renderer()->gi_system().gi_resolve_material_system()->add_gi_diffuse(lpv_resolve_material_system_->output(0));

        return true;
    }

    bool update(mhe::game::Engine& engine) override
    {
        if (engine.event_manager().check_bind("debug_lpv"))
        {
            debug_mode_on_ ^= 1;
            //engine.renderer()->set_debug_mode(debug_mode_on_ ? Renderer::renderer_debug_mode_posteffect : Renderer::renderer_debug_mode_none,
            //    lpv_resolve_material_system_->id());
        }

        const KeyboardDevice* keyboard = engine.event_manager().keyboard();
        if (keyboard->is_key_pressed(KeyboardDevice::key_o))
            lpv_material_system_->settings().use_occlusion ^= 1;
        if (keyboard->is_key_pressed(KeyboardDevice::key_l))
            lpv_resolve_material_system_->settings().damping ^= 1;
        return true;
    }

    void before_draw(game::Engine& /*engine*/) override
    {
        // RSM data has been updated
        RSMData data;
        rsm_material_system_->rsm_data(data);
        lpv_material_system_->set_rsm_data(data);
    }
private:
    RSMMaterialSystem* rsm_material_system_;
    LPVMaterialSystem* lpv_material_system_;
    LPVResolveMaterialSystem* lpv_resolve_material_system_;
    bool debug_mode_on_;
};

int main(int /*argc*/, char** /*argv*/)
{
    mhe::app::Application app("14_lpv");
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
    config.render_config_filename = mhe::utils::path_join(config.assets_path, mhe::string("render_without_postprocess_pbr.xml"));
    app.init(config);

    mhe::game::GameSceneDesc desc;
    GameScene* game_scene = new GameScene;
    game_scene->init(app.engine(), desc);
    app.engine().set_game_scene(game_scene);

    mhe::PerspectiveCameraParameters camera_parameters;
    camera_parameters.fov = 60.0f;
    camera_parameters.znear = 0.5f;
#ifndef SPONZA
    camera_parameters.zfar = 40.0f;
#else
    camera_parameters.zfar = 3000.0f;
#endif
    mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
        mhe::vec3(0, 1, 5), mhe::vec3(0.0f, mhe::pi, 0));
    camera_controller->set_move_speed(75.0f);
    camera_controller->set_rotation_speed(50.0f);
    app.engine().scene().set_camera_controller(camera_controller);

    return app.run();
}
