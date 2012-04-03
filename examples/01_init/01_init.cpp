#include "mhe.hpp"
#include "platform/platform_system.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	mhe::utils::create_standart_log();
	mhe::impl::start_platform();
	mhe::game::Engine engine;
	if (!engine.init(800, 600, 32))
	{
		std::cerr << "Can't init engine\n";
		return 1;
	}
	std::cout << "Engine initialized sucessfully\n";
	engine.run();

	mhe::impl::stop_platform();
	
	return 0;
}
