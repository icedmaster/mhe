#include "mhe.hpp"

class TestApplication : public mhe::Application2D
{
private:
	void init_impl()
	{
		std::cout << "TestApp init\n";
	}

	void deinit_impl()
	{
		std::cout << "TestApp deinit\n";
	}

	int run_impl(const boost::shared_ptr<mhe::game::GameScene>&)
	{
		std::cout << "run\n";
		return 0;
	}
};

int main(int, char**)
{
	mhe::ApplicationConfig config;
	if (!mhe::load_config(config))
	{
		config.width = 800;
		config.height = 600;
		config.bpp = 32;
		config.fullscreen = false;
	}
	TestApplication app;
	app.init("07_application", config);
	return app.run(nullptr);
}
