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
		prm.emit_count.set(40);
		prm.emit_interval.set(200);
		prm.size.set(20, 20);
		prm.end_size = 20;
		prm.color.set(mhe::colorf(1.0, 1.0, 1.0, 1.0),
    			      mhe::colorf(1.0, 1.0, 1.0, 1.0));
		prm.end_color = mhe::colorf(0.0, 1.0, 0.0, 0.0);
		prm.speed.set(mhe::v3d(0.5, 0.5, 0.0),
					  mhe::v3d(0.8, 0.8, 0.0));
		prm.accel.set(mhe::v3d(1.0, 1.0, 0.0),
					  mhe::v3d(1.0, 1.0, 0.0));
		prm.angle.set(mhe::deg_to_rad(30), mhe::deg_to_rad(45.0));
		prm.spread.set(mhe::deg_to_rad(0), mhe::deg_to_rad(360));
		prm.lifetime.set(4000, 4200);
		boost::shared_ptr<mhe::PointParticleEmitter> emitter(new mhe::PointParticleEmitter());
		emitter->init_with_parameters(prm);
		boost::shared_ptr<mhe::ParticleSystem> ps(new mhe::ParticleSystem(mhe::v3d(400, 300, 0)));
		ps->set_emitter(emitter);
		ps->set_texture(get_engine()->context().texture_manager().get("../../../assets/star.png"));
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
	engine.context().driver()->set_projection_matrix(proj);
	boost::shared_ptr<TestGameScene> game_scene(new TestGameScene(&engine));
	game_scene->init("", nullptr);
	engine.set_game_scene(game_scene);
	engine.run();

	mhe::impl::stop_platform();

	return 0;
}
