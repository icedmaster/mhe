#include "mhe.hpp"
#include "render/rsm_material_system.hpp"
#include "render/lpv_material_system.hpp"

const char* mesh_name = "test-scene-simple.bin";

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
        light.shading().intensity = 5.0f;
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(0.0f, -mhe::pi_2 * 1.3f, mhe::pi_2 * 0.7f));
        light.set_type(mhe::Light::directional);
        light.desc().cast_shadows = true;
        light.desc().shadowmap_bias = 0.01f;
        light_instance.enabled = true;

        mhe::NodeInstance& node = engine.scene().create_node();
        mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string(mesh_name), engine.context(), engine.scene_context());

        for (size_t i = 0, size = node.mesh.mesh.parts.size(); i < size; ++i)
        {
            engine.context().material_manager.material_data(node.mesh.mesh.parts[i].material_id).render_data.glossiness = 0.5f;
        }

        const string rsm_name("rsm");
        MaterialSystemContext material_system_context;
        material_system_context.priority = DeferredRenderer::deferred_renderer_base_priority - 2;
        material_system_context.shader_name = rsm_name;
        material_system_context.instance_name = rsm_name;
        material_system_context.material_instances_number = 0;
        engine.context().initialization_parameters.add(rsm_name, material_system_context);

        RSMMaterialSystem* rsm_material_system = static_cast<RSMMaterialSystem*>(create(engine.context(), rsm_name, rsm_name));
        engine.renderer()->set_material_system_to_process(rsm_material_system);

        const string lpv_name("lpv");
        material_system_context.priority = DeferredRenderer::deferred_renderer_base_priority - 1;
        material_system_context.shader_name = lpv_name;
        material_system_context.instance_name = lpv_name;
        material_system_context.material_instances_number = 0;
        material_system_context.options.add(string("injection_shader"), lpv_name);
        engine.context().initialization_parameters.add(lpv_name, material_system_context);

        LPVMaterialSystem* lpv_material_system = static_cast<LPVMaterialSystem*>(create(engine.context(), lpv_name, lpv_name));
        engine.renderer()->set_material_system_to_process(lpv_material_system);
        lpv_material_system->set_gbuffer(rsm_material_system->gbuffer());

        return true;
    }

    bool update(mhe::game::Engine& /*engine*/) override
    {
        return true;
    }
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
    config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_without_postprocess.xml");
    app.init(config);

    mhe::game::GameSceneDesc desc;
    GameScene* game_scene = new GameScene;
    game_scene->init(app.engine(), desc);
    app.engine().set_game_scene(game_scene);

    mhe::PerspectiveCameraParameters camera_parameters;
    camera_parameters.fov = 60.0f;
    camera_parameters.znear = 0.5f;
    camera_parameters.zfar = 50.0f;
    mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
        mhe::vec3(0, 1, 5), mhe::vec3(0.0f, mhe::pi, 0));
    camera_controller->set_move_speed(75.0f);
    camera_controller->set_rotation_speed(50.0f);
    app.engine().scene().set_camera_controller(camera_controller);

    return app.run();
}
