#include "mhe.hpp"
#include "render/vct_material_system.hpp"

using namespace mhe;

class GameScene : public mhe::game::GameScene
{
public:
    bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
    {
        light_id_ = mhe::create_light(engine.context(), engine.scene(), mhe::utils::path_join(mhe::app::default_objects_path(), mhe::string("17-vct-light.xml"))).id;

        mhe::NodeInstance& node = engine.scene().create_node();
        mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("pbr-test-simple.mesh"), engine.context(), engine.scene_context());
        //mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("sponza.mesh"), engine.context(), engine.scene_context());

        const string voxelize_name = mhe::VoxelizeMaterialSystem::material_name();
        MaterialSystemContext material_system_context;
        material_system_context.instance_name = voxelize_name;
        material_system_context.shader_name = voxelize_name;
        engine.context().initialization_parameters.add(voxelize_name, material_system_context);
        voxelize_material_system_ = create_material_system<mhe::VoxelizeMaterialSystem>(engine.context(), voxelize_name, voxelize_name);
        voxelize_material_system_->set_priority(1);
        engine.renderer()->set_material_system_to_process(voxelize_material_system_);

        return true;
    }

    bool update(mhe::game::Engine& engine) override
    {
        if (engine.event_manager().keyboard()->is_key_pressed(mhe::KeyboardDevice::key_r))
        {
            mhe::reload_light(engine.context(), engine.scene(), engine.scene().scene_context().light_pool.get(light_id_));
        }
        return true;
    }
private:
    mhe::LightInstance::IdType light_id_;
    mhe::VoxelizeMaterialSystem* voxelize_material_system_;
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
