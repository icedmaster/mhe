#include "mhe.hpp"
#include <iostream>

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("01_init");
	mhe::app::ApplicationConfig config;
	config.width = 800;
	config.height = 600;
	config.bpp = 32;
	config.fullscreen = false;
	app.init(config);
	return app.run();
}
