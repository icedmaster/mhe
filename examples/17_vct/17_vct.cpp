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

        return true;
    }

    bool update(mhe::game::Engine& engine) override
    {
        return true;
    }
};

int main(int /*argc*/, char** /*argv*/)
{
    mhe::app::Application app("17_vct");
    mhe::app::ApplicationConfig config;
    config.width = 1280;
    config.height = 720;
    config.bpp = 32;
    config.fullscreen = false;

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
