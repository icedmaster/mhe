#include "mhe.hpp"

class TestApplication : public mhe::app::Application2D
{
public:
	TestApplication(const std::string& name) :
		mhe::app::Application2D(name)
	{}
private:
	void init_impl()
	{
		std::cout << "TestApp init\n";
	}

	void deinit_impl()
	{
		std::cout << "TestApp deinit\n";
	}

	int run_impl()
	{
		std::cout << "run\n";
		deinit();
		return 0;
	}
};

int main(int, char**)
{
	mhe::app::ApplicationConfig config;
	if (!mhe::app::load_config(config))
	{
		config.width = 800;
		config.height = 600;
		config.bpp = 32;
		config.fullscreen = false;
	}
	TestApplication app("07_application");
	app.init(config, nullptr);
	return app.run();
}
