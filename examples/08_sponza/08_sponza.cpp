#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
    bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
    {
        x_ = y_ = z_ = 0.0f;
        x_ = mhe::pi_2;

        keyboard_ = engine.event_manager().keyboard();
        
        mhe::NodeInstance& node = engine.scene().create_node();
        mhe::load_node<mhe::GBufferFillMaterialSystem>(node, mhe::string("sponza.mesh"), engine.context(), engine.scene_context());
        //mhe::MaterialId floor_id;
        //engine.context().material_manager.id_by_name(floor_id, mhe::string("floor"));
        //mhe::MaterialData& material = engine.context().material_manager.material_data(floor_id);
        //material.render_data.glossiness = 1.0f;

        node.mesh.instance_parts[4].flags &= ~mhe::MeshPartInstance::cast_reflection;

        init_lighting(engine);

        mhe::BlurMaterialSystem* ssr_blur = engine.context().material_systems.get<mhe::BlurMaterialSystem>(mhe::string("ssr_blur"));
        if (ssr_blur != nullptr)
        {
            ssr_blur->settings().quality = mhe::BlurMaterialSystem::quality_high;
            ssr_blur->settings().size = 2.0f;
        }

        ssr_blur = engine.context().material_systems.get<mhe::BlurMaterialSystem>(mhe::string("ssao_blur"));
        if (ssr_blur != nullptr)
        {
            ssr_blur->settings().quality = mhe::BlurMaterialSystem::quality_high;
            ssr_blur->settings().size = 2.0f;
        }

        mhe::CSMDepthRenderingMaterialSystem* csm = engine.context().material_systems.get<mhe::CSMDepthRenderingMaterialSystem>();
        if (csm != nullptr)
            csm->settings().distance_calculation_mode = mhe::CSMDepthRenderingMaterialSystem::aabb_extents;

        mhe::CubemapCreationMaterialSystem* cubemap_creation_material_system = new mhe::CubemapCreationMaterialSystem;
        mhe::MaterialSystemContext material_system_context;
        material_system_context.shader_name = "render_simple";
        cubemap_creation_material_system->init(engine.context(), material_system_context);
        engine.context().material_systems.add(cubemap_creation_material_system);
        engine.context().materials[cubemap_creation_material_system->id()].resize(initial_material_instances_number);

        engine.context().driver.begin_render();
        cubemap_creation_material_system->render_cubemap(engine.context(), engine.scene_context(), engine.render_context(), mhe::vec3(0, 100, 0), 1000.0f);
        engine.context().driver.end_render();
        engine.context().window_system.swap_buffers();
        engine.render_context().space_grid.set_global_cubemap(cubemap_creation_material_system->texture());

        return true;
    }

    bool update(mhe::game::Engine& engine)
    {
        ImGui::Begin("light");
        ImGui::SliderAngle("x", &x_);
        ImGui::SliderAngle("y", &y_);
        ImGui::SliderAngle("z", &z_);
        ImGui::End();

        mhe::set_light_rotation(engine.scene_context(), directional_lights_[0], mhe::quatf(x_, y_, z_));

        if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_p))
        {
            mhe::SSRMaterialSystem* ssr_material_system = engine.context().material_systems.get<mhe::SSRMaterialSystem>();
            if (ssr_material_system != nullptr)
                ssr_material_system->settings().use_probes ^= 1;
        }

        if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_0))
            light_type_ = mhe::spot;
        else if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_2))
            light_type_ = mhe::directional;
        else return true;
        update_lights(engine);
        return true;
    }
private:
    void init_lighting(mhe::game::Engine& engine)
    {
        init_directional_lights(engine);
    }

    void init_spot_lights(mhe::game::Engine& engine)
    {
        mhe::LightInstance& light_instance = engine.scene().create_light(mhe::spot);
        mhe::res::Light& light = light_instance.dblight;
        light.diffuse_color = mhe::color_rgb_green;
        light.specular_color = mhe::color_rgb_white;
        mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 20, 0));
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
        light.attenuation = 0.2f;
        light.spot_attenuation = 0.5f;
        light.angle = mhe::deg_to_rad(30.0f);
        light.shadowmap_znear = 0.1f;
        light.shadowmap_zfar = 50.0f;
        light.cast_shadows = false;

        mhe::LightInstance& light_instance2 = engine.scene().create_light(mhe::spot);
        mhe::res::Light& light2 = light_instance2.dblight;
        light.diffuse_color = mhe::color_rgb_green;
        light.specular_color = mhe::color_rgb_white;
        mhe::set_light_position(engine.scene_context(), light_instance2.id, mhe::vec3(0, 1, 20));
        mhe::set_light_rotation(engine.scene_context(), light_instance2.id, mhe::quatf(0.0f, mhe::pi, 0.0f));
        light.attenuation = 0.2f;
        light.spot_attenuation = 0.5f;
        light.angle = mhe::deg_to_rad(30.0f);
        light.shadowmap_znear = 0.1f;
        light.shadowmap_zfar = 50.0f;
        light.cast_shadows = true;

        spot_lights_[0] = light_instance.id;
        spot_lights_[1] = light_instance2.id;
    }

    void init_directional_lights(mhe::game::Engine& engine)
    {
        mhe::LightInstance& light_instance = engine.scene().create_light(mhe::directional);
        mhe::res::Light& light = light_instance.dblight;
        light.diffuse_color = mhe::vec3(0.78f, 1.0f, 0.716f);
        light.specular_color = mhe::color_rgb_white;
        light.intensity = 5.0f;
        mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 2000, 0));
        light.shadowmap_znear = 10.0f;
        light.shadowmap_zfar = 2200.0f;
        light.cast_shadows = true;
        light.shadowmap_bias = 0.002f;
        light_instance.enabled = true;

        directional_lights_[0] = light_instance.id;
    }

    void update_lights(mhe::game::Engine& engine)
    {
        mhe::LightInstance::IdType for_enable[2];
        mhe::LightInstance::IdType for_disable[2];

        switch (light_type_)
        {
        case mhe::spot:
            std::copy(spot_lights_, spot_lights_ + 2, for_enable);
            std::copy(directional_lights_, directional_lights_ + 2, for_disable);
            break;

        case mhe::directional:
            std::copy(directional_lights_, directional_lights_ + 2, for_enable);
            std::copy(spot_lights_, spot_lights_ + 2, for_disable);
            break;

        default: break;
        }

        for (int i = 0; i < 2; ++i)
            engine.scene_context().light_pool.get(for_enable[i]).enabled = true;

        for (int i = 0; i < 2; ++i)
            engine.scene_context().light_pool.get(for_disable[i]).enabled = false;
    }

    mhe::LightInstance::IdType spot_lights_[2];
    mhe::LightInstance::IdType directional_lights_[2];
    const mhe::KeyboardDevice* keyboard_;
    int light_type_;
    float x_, y_, z_;
};

int main(int /*argc*/, char** /*argv*/)
{
    mhe::app::Application app("08_sponza");
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
    config.render_config_filename = mhe::utils::path_join(config.assets_path, mhe::string("render.xml"));
    app.init(config);

    app.engine().renderer()->set_ambient_color(mhe::color_white * 0.3f);

    mhe::game::GameSceneDesc desc;
    GameScene* game_scene = new GameScene;
    game_scene->init(app.engine(), desc);
    app.engine().set_game_scene(game_scene);

    mhe::PerspectiveCameraParameters camera_parameters;
    camera_parameters.fov = 60.0f;
    camera_parameters.znear = 1.0f;
    camera_parameters.zfar = 3000.0f;
    mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
        mhe::vec3(0, 10, 10), mhe::vec3(mhe::deg_to_rad(-30.0f), mhe::pi_2, 0));
    camera_controller->set_move_speed(500.0f);
    app.engine().scene().set_camera_controller(camera_controller);
    return app.run();
}
