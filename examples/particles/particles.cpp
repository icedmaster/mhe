#include "mhe.hpp"
#include <iostream>

class TestGameScene : public mhe::game::GameScene
{
public:
	TestGameScene(mhe::game::Engine* engine) :
		mhe::game::GameScene(engine)
	{}

private:
	bool init_impl(const std::string& /*arg*/, void* /*prm*/)
	{
		mhe::ParticleEmitterParameters prm;
		prm.min_emit_count = 40;
		prm.max_emit_count = 40;
		prm.min_emit_interval = 1500;
		prm.max_emit_interval = 1500;
		prm.min_size = 5;
		prm.max_size = 7;
		prm.size_delta = 0;
		prm.min_color = mhe::colorf(0.75, 0.75, 0.75, 1.0);
		prm.max_color = mhe::colorf(1.0, 1.0, 1.0, 1.0);
		prm.color_delta = mhe::colorf(0.05, 0.05, 0.05, 0.0);
		prm.min_speed = mhe::v3d(1.0, 1.0, 0.0);
		prm.max_speed = mhe::v3d(2.0, 2.0, 0.0);
		prm.min_accel = mhe::v3d(1.1, 1.1, 0.0);
		prm.max_accel = mhe::v3d(1.3, 1.3, 0.0);
		prm.min_lifetime = 2000;
		prm.max_lifetime = 2200;
		boost::shared_ptr<mhe::DefaultParticleEmitter> emitter(new mhe::DefaultParticleEmitter());
		emitter->init_with_parameters(prm);
		boost::shared_ptr<mhe::ParticleSystem> ps(new mhe::ParticleSystem(mhe::v3d(10, 10, 0)));
		ps->set_emitter(emitter);
		get_scene()->add(ps);
		ps->start();
		return true;
	}
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::utils::create_standart_log();
	mhe::utils::init_randomizer();
	mhe::impl::start_platform();
	mhe::game::Engine engine;
	if (!engine.init(800, 600, 32))
	{
		std::cerr << "Can't init engine\n";
		return 1;
	}
	std::cout << "Engine initialized sucessfully\n";
	mhe::matrixf proj;
	proj.set_ortho(0, 800, 0, 600, -1, 1);
	engine.getDriver()->set_projection_matrix(proj);
	boost::shared_ptr<TestGameScene> game_scene(new TestGameScene(&engine));
	game_scene->init("", nullptr);
	engine.setGameScene(game_scene);
	engine.run();

	mhe::impl::stop_platform();

	return 0;
}
