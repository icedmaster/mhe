#include "mhe.hpp"

#include <render/fog.hpp>

using namespace mhe;

class GameScene : public mhe::game::GameScene
{
public:
    bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
    {
        mhe::LightInstance& light_instance = engine.scene().create_light();
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
        mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("sponza.bin"), engine.context(), engine.scene_context());

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

        engine.renderer()->posteffect_system().add(engine.context(), posteffect_node_desc);
        HeightFogMaterialSystem* fog_material_system = engine.context().material_systems.get<HeightFogMaterialSystem>();
        fog_material_system->settings().start = 100.0f;
        fog_material_system->settings().density = 0.002f;
        fog_material_system->settings().color.set(0.5f, 0.6f, 0.7f, 0.0f);

        return true;
    }

    bool update(mhe::game::Engine& engine) override
    {
        return true;
    }

    void before_draw(game::Engine& /*engine*/) override
    {
    }
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
    config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_without_postprocess.xml");
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
