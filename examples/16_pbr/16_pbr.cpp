#include "mhe.hpp"

using namespace mhe;

class GameScene : public mhe::game::GameScene
{
public:
    bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
    {
        mhe::LightInstance& light_instance = engine.scene().create_light(mhe::directional);
        mhe::res::Light& light = light_instance.dblight;
        light.diffuse_color = mhe::color_rgb_white;
        light.specular_color = mhe::color_rgb_white;
        light.intensity = 3.0f;
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(mhe::pi_2 * 0.75f, mhe::deg_to_rad(15.0f), 0.0f));
        light.cast_shadows = true;
        light.shadowmap_bias = 0.01f;
        light_instance.enabled = true;

        mhe::NodeInstance& node = engine.scene().create_node();
        mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("pbr-test-simple.mesh"), engine.context(), engine.scene_context());

        BloomMaterialSystem* bloom_material_system = engine.context().material_systems.get<BloomMaterialSystem>();
        bloom_material_system->settings().intensity = 0.0f;

        engine.context().texture_manager.get(skyboxes_[0], string("cubemaps/test.cubemap"));
        engine.context().texture_manager.get(skyboxes_[1], string("cubemaps/yokohama/yokohama.cubemap"));
        engine.context().texture_manager.get(skyboxes_[2], string("cubemaps/ice-river/ice-river.cubemap"));

        return true;
    }

    bool update(mhe::game::Engine& engine) override
    {
        ImGui::Begin("PBR");
        MaterialData* materials = engine.context().material_data_pool.all_objects();
        for (size_t i = 0, size = engine.context().material_data_pool.size(); i < size; ++i)
        {
            ImGui::LabelText("", "%u", i);
            ImGui::SliderFloat("R", &materials[i].render_data.roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("M", &materials[i].render_data.metalness, 0.0f, 1.0f);
        }
        ImGui::End();

        const KeyboardDevice* keyboard = engine.event_manager().keyboard();
        if (keyboard->is_key_pressed(KeyboardDevice::key_1))
            update_skybox(engine, 0);
        else if (keyboard->is_key_pressed(KeyboardDevice::key_2))
            update_skybox(engine, 1);
        else if (keyboard->is_key_pressed(KeyboardDevice::key_3))
            update_skybox(engine, 2);
        return true;
    }

    void update_skybox(mhe::game::Engine& engine, int index)
    {
        engine.renderer()->set_skybox_cubemap(skyboxes_[index]);
    }

    TextureInstance skyboxes_[3];
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

    config.render_config_filename = mhe::utils::path_join(config.assets_path, mhe::string("render_without_postprocess_pbr.xml"));
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
