#include "mhe.hpp"

int main(int argc, char** argv)
{
	mhe::utils::createStandartLog();
    mhe::utils::init_randomizer();

	mhe::game::Engine engine;
	if (!engine.init(800, 600, 32))
	{
		mhe::utils::global_log::instance().write("engine init failed");
		return 1;
	}

	engine.run();
	return 0;
}
