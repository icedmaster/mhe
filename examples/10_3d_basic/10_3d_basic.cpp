#include "mhe.hpp"

#include <iostream>

class TestNode : public mhe::Node
{
private:
	void draw_impl(mhe::Context& context)
	{
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(20.0, 0.0, 0.0);
		glEnd();

		glColor4f(0.0, 1.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 20.0, 0.0);
		glEnd();

		glColor4f(0.0, 0.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 20.0);
		glEnd();
	}
};

class Cube : public mhe::Node
{
public:
	Cube(float size)
	{
		setup_buffers(size);
	}
private:
	void setup_buffers(float size)
	{
		mhe::mesh_ptr mesh(new mhe::Mesh);
		mhe::Mesh::vertexes_vector& v = mesh->vertexes;
		v.resize(24);
		v[0] = 0; v[1] = 0; v[2] = 0;   // 0
		v[3] = 0; v[4] = size; v[5] = 0;  // 1
		v[6] = size; v[7] = size; v[8] = 0; // 2
		v[9] = size; v[10] = 0; v[11] = 0;  // 3
		v[12] = 0; v[13] = 0; v[14] = -size; // 4
		v[15] = 0; v[16] = size; v[17] = -size; // 5
		v[18] = size; v[19] = size; v[20] = -size; // 6
		v[21] = size; v[22] = 0; v[23] = -size; // 7
		mhe::Mesh::indexes_vector& i = mesh->indexes;
		i.resize(36);
		// front
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 2; i[4] = 3; i[5] = 0;	
		// left
		i[6] = 0; i[7] = 1; i[8] = 5;
		i[9] = 5; i[10] = 4; i[11] = 0;
		// right
		i[12] = 3; i[13] = 2; i[14] = 6;
		i[15] = 6; i[16] = 7; i[17] = 3;
		// back
		i[18] = 4; i[19] = 5; i[20] = 6;
		i[21] = 6; i[22] = 7; i[23] = 4;
		// top
		i[24] = 1; i[25] = 5; i[26] = 6;
		i[27] = 6; i[28] = 2; i[29] = 1;
		// bottom
		i[30] = 0; i[31] = 4; i[32] = 7;
		i[33] = 7; i[34] = 3; i[35] = 0;

		Renderable::set_mesh(mesh);
	}
};

class TestScene : public mhe::game::GameScene
{
public:
	TestScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine),
		prev_time_(0)
	{}

	bool process_key(const mhe::Event* event)
	{
		const mhe::KeyboardEvent* ke = static_cast<const mhe::KeyboardEvent*>(event);
		mhe::v3d delta;
		switch (ke->sym())
		{
			case mhe::KeyboardDevice::key_a:
				delta.set_x(-0.2);
				break;

			case mhe::KeyboardDevice::key_d:
				delta.set_x(0.2);
				break;
				
			case mhe::KeyboardDevice::key_w:
				delta.set_y(0.2);
				break;

			case mhe::KeyboardDevice::key_s:
				delta.set_y(-0.2);
				break;

			default: std::cout << "Key pressed:" << ke->sym() << std::endl;
		}
		scene()->active_camera()->translate_by(delta);
		return true;
	}
private:
	bool init_impl(const mhe::utils::PropertiesList&)
	{
		mhe::Camera* camera = new mhe::Camera;
		mhe::matrixf m;
		m.set_perspective(60, 800 / 600, 1, 100);
		camera->set_projection(m);		
		camera->set(mhe::vector3<float>(0, 0, 10), mhe::vector3<float>(), mhe::y_axis);
		camera->enable_each_frame_updating(true);
		scene()->add_camera(camera, true);
		
		get_engine()->font_manager().set_path("../../../assets/fonts/");
		stats_component_.reset(new mhe::game::utils::StatsComponent(scene(), engine()));
		engine()->component_manager().add(stats_component_);

		scene()->add(new TestNode);
		Cube* cube = new Cube(3);
		//cube->set_texture(engine()->context().texture_manager().get(mhe::default_resource_name));
		cube->rotate_by(0, 15, 0);
		scene()->add(cube);

		engine()->event_manager().add_listener(new mhe::DelegateEventListener(mhe::keyboard_event_type,
																			  mhe::KeyboardEvent::key_down, mhe::Event::any_event,
																			  mhe::create_delegate(this, &TestScene::process_key)));
		mouse_ = engine()->event_manager().get_device<mhe::MouseDevice>();
		return true;
	}

	bool process_impl()
	{
		cmn::uint now = mhe::utils::get_current_tick();
		if (!prev_time_) prev_time_ = now;
		else if (now >= (prev_time_ + 1000))
		{
			prev_time_ = now;
			std::cout << mhe::utils::Profiler::instance() << std::endl;
			mhe::utils::Profiler::instance().reset();
		}
		
		return true;
	}

	void draw_impl()
	{
	}

	mhe::game::component_ptr stats_component_;
	cmn::uint prev_time_;
	boost::shared_ptr<mhe::MouseDevice> mouse_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("10_3d_basic");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
	app.init(config);

	// set scene
	boost::shared_ptr<TestScene> scene(new TestScene(&app.engine()));
	app.engine().set_game_scene(scene);

	app.engine().event_manager().add_device(new mhe::MouseDevice("mouse"));
	app.engine().event_manager().add_device(new mhe::KeyboardDevice("keyb"));

	scene->init();

	// run application
	return app.run();
}
