#include "test/test_sound.hpp"
#include "test/test_engine.hpp"
#include "test/test_scene.hpp"
#include "test/test_aspect.hpp"
#include "test/test_wav_sound.hpp"
#include "test/test_python.hpp"

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
