#include "mhe.hpp"

class GameScene : public mhe::game::GameScene
{
public:
    bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/)
    {
        keyboard_ = engine.event_manager().keyboard();
        
        mhe::NodeInstance& node = engine.scene().create_node();
        engine.context().mesh_manager.get_instance(node.mesh, mhe::string("sphere.bin"));
        mhe::Transform& transform = engine.scene().transform_pool().get(node.transform_id).transform;
        transform.scale_to(mhe::vec3(2, 2, 2));
        node.cast_shadow = true;
                node.cast_reflection = false;
        
        mhe::NodeInstance& plane = engine.scene().create_node();
        mhe::utils::create_plane(plane, engine.context());
        mhe::Transform& plane_transform = engine.scene().transform_pool().get(plane.transform_id).transform;
        plane_transform.scale_to(mhe::vec3(50, 50, 50));
        plane_transform.translate_by(mhe::vec3(0, 0, -20));
        plane.receive_shadow = true;

        mhe::NodeInstance& floor = engine.scene().create_node();
        mhe::utils::create_plane(floor, engine.context());
        mhe::Transform& floor_transform = engine.scene().transform_pool().get(floor.transform_id).transform;
        floor_transform.scale_to(mhe::vec3(50, 50, 50));
        floor_transform.rotate_by(mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
        floor_transform.translate_by(mhe::vec3(0.0f, -10.0f, 0.0f));
        floor.receive_shadow = true;

        mhe::NodeInstance& rotating_sphere = engine.scene().create_node();
        engine.context().mesh_manager.get_instance(rotating_sphere.mesh, mhe::string("sphere.bin"));
        rotated_sphere_transform_ = &engine.scene().transform_pool().get(rotating_sphere.transform_id);
        mhe::Transform& rotating_sphere_transform = rotated_sphere_transform_->transform;
        rotating_sphere_transform.scale_to(mhe::vec3(0.7f, 0.7f, 0.7f));
        rotating_sphere_transform.translate_by(mhe::vec3(0.0f, 5.0f, 15.0f));
        rotating_sphere.receive_shadow = true;
        rotating_sphere.cast_shadow = true;

        mhe::MaterialInitializationData glossy_material;
        glossy_material.name = "glossy";
        glossy_material.textures[mhe::albedo_texture_unit] = "metal.tga";
        glossy_material.render_data.specular_shininess = 5.0f;
        glossy_material.render_data.glossiness = 1.0f;

        mhe::GBufferFillMaterialSystem* material_system = engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>();
        mhe::setup_node(node, material_system, engine.context(), engine.scene_context(), glossy_material);
        mhe::setup_node(plane, material_system, engine.context(), engine.scene_context(), glossy_material);
        mhe::setup_node(floor, material_system, engine.context(), engine.scene_context(), mhe::string("test.tga"));
        mhe::setup_node(rotating_sphere, material_system, engine.context(), engine.scene_context(), mhe::string("test.tga"));

        init_lighting(engine);

        mhe::MaterialSystemContext cubemap_creation_context;
        cubemap_creation_context.shader_name = "render_simple";
        cubemap_creation_material_system_ = new mhe::CubemapCreationMaterialSystem;
        cubemap_creation_material_system_->init(engine.context(), cubemap_creation_context);
        engine.context().material_systems.add(cubemap_creation_material_system_);
        engine.context().materials[cubemap_creation_material_system_->id()].resize(initial_material_instances_number);
        return true;
    }

    bool update(mhe::game::Engine& engine)
    {
        if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_0))
            light_type_ = mhe::Light::spot;
        else if (keyboard_->is_key_pressed(mhe::KeyboardDevice::key_2))
            light_type_ = mhe::Light::directional;
        else return true;
        update_lights(engine);
        return true;
    }

    void before_draw(mhe::game::Engine& engine) override
    {
        cubemap_creation_material_system_->render_cubemap(engine.context(), engine.scene_context(), engine.render_context(), mhe::vec3( 0.0f, 0.0f, 2.0f ), 50.0f);
        engine.render_context().space_grid.set_global_cubemap(cubemap_creation_material_system_->texture());
    }
private:
    void init_lighting(mhe::game::Engine& engine)
    {
        init_spot_lights(engine);
        init_directional_lights(engine);

        light_type_ = mhe::Light::spot;
        update_lights(engine);
    }

    void init_spot_lights(mhe::game::Engine& engine)
    {
        mhe::LightInstance& light_instance = engine.scene().create_light();
        mhe::Light& light = light_instance.light;
        light.shading().diffuse = mhe::color_green * 0.5f;
        light.shading().specular = mhe::color_white;
        mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 20, 0));
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
        light.desc().spot.attenuation_a = 0.005f;
        light.desc().spot.attenuation_b = 0.01f;
        light.desc().spot.angle = mhe::deg_to_rad(30.0f);
        light.desc().spot.angle_attenuation = 0.5f;
        light.desc().spot.spot_shadowmap_projection_znear = 0.1f;
        light.desc().spot.spot_shadowmap_projection_zfar = 50.0f;
        light.desc().cast_shadows = false;
        light.set_type(mhe::Light::spot);

        mhe::LightInstance& light_instance2 = engine.scene().create_light();
        mhe::Light& light2 = light_instance2.light;
        light2.shading().diffuse = mhe::color_blue * 0.7f;
        light2.shading().specular = mhe::color_white;
        mhe::set_light_position(engine.scene_context(), light_instance2.id, mhe::vec3(0, 1, 20));
        mhe::set_light_rotation(engine.scene_context(), light_instance2.id, mhe::quatf(0.0f, mhe::pi, 0.0f));
        light2.desc().spot.attenuation_a = 0.005f;
        light2.desc().spot.attenuation_b = 0.01f;
        light2.desc().spot.angle = mhe::deg_to_rad(30.0f);
        light2.desc().spot.angle_attenuation = 0.5f;
        light2.desc().spot.spot_shadowmap_projection_znear = 0.1f;
        light2.desc().spot.spot_shadowmap_projection_zfar = 50.0f;
        light2.desc().cast_shadows = true;
        light2.set_type(mhe::Light::spot);

        spot_lights_[0] = light_instance.id;
        spot_lights_[1] = light_instance2.id;
    }

    void init_directional_lights(mhe::game::Engine& engine)
    {
        mhe::LightInstance& light_instance = engine.scene().create_light();
        mhe::Light& light = light_instance.light;
        light.shading().diffuse = mhe::vec4(0.0f, 0.3f, 0.0f, 1.0f);
        light.shading().specular = mhe::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        mhe::set_light_position(engine.scene_context(), light_instance.id, mhe::vec3(0, 100, 0));
        mhe::set_light_rotation(engine.scene_context(), light_instance.id, mhe::quatf(-mhe::pi_2, 0.0f, 0.0f));
        light.set_type(mhe::Light::directional);
        light.desc().directional.directional_shadowmap_projection_znear = 0.1f;
        light.desc().directional.directional_shadowmap_projection_zfar = 120.0f;
        light.desc().directional.directional_shadowmap_projection_height = 100.0;
        light.desc().directional.directional_shadowmap_projection_width = 100.0;
        light.desc().cast_shadows = true;

        mhe::LightInstance& light_instance2 = engine.scene().create_light();
        mhe::Light& light2 = light_instance2.light;
        light2.shading().diffuse = mhe::vec4(0.0f, 0.0f, 0.3f, 1.0f);
        light2.shading().specular = mhe::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        mhe::set_light_position(engine.scene_context(), light_instance2.id, mhe::vec3(0, 1, 100));
        mhe::set_light_rotation(engine.scene_context(), light_instance2.id, mhe::quatf(0.0f, mhe::pi, 0.0f));
        light2.set_type(mhe::Light::directional);
        light2.desc().directional.directional_shadowmap_projection_znear = 0.1f;
        light2.desc().directional.directional_shadowmap_projection_zfar = 120.0f;
        light2.desc().directional.directional_shadowmap_projection_height = 100.0;
        light2.desc().directional.directional_shadowmap_projection_width = 100.0;
        light2.desc().cast_shadows = true;

        directional_lights_[0] = light_instance.id;
        directional_lights_[1] = light_instance2.id;
    }

    void update_lights(mhe::game::Engine& engine)
    {
        mhe::LightInstance::IdType for_enable[2];
        mhe::LightInstance::IdType for_disable[2];

        switch (light_type_)
        {
        case mhe::Light::spot:
            std::copy(spot_lights_, spot_lights_ + 2, for_enable);
            std::copy(directional_lights_, directional_lights_ + 2, for_disable);
            break;

        case mhe::Light::directional:
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

    mhe::CubemapCreationMaterialSystem* cubemap_creation_material_system_;
    mhe::LightInstance::IdType spot_lights_[2];
    mhe::LightInstance::IdType directional_lights_[2];
    const mhe::KeyboardDevice* keyboard_;
    int light_type_;
    mhe::TransformInstance* rotated_sphere_transform_;
};

int main(int /*argc*/, char** /*argv*/)
{
    mhe::app::Application app("09_reflections");
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
    config.render_config_filename = mhe::utils::path_join(config.assets_path, "render.xml");
    app.init(config);

    mhe::game::GameSceneDesc desc;
    GameScene* game_scene = new GameScene;
    game_scene->init(app.engine(), desc);
    app.engine().set_game_scene(game_scene);

    mhe::PerspectiveCameraParameters camera_parameters;
    camera_parameters.fov = 60.0f;
    camera_parameters.znear = 0.1f;
    camera_parameters.zfar = 100.0f;
    app.engine().scene().set_camera_controller(new mhe::game::FPSCameraController(app.engine(), camera_parameters,
        mhe::vec3(0, 1, 10), mhe::vec3(0, mhe::pi, 0)));
    return app.run();
}
