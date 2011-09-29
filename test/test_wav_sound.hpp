#ifndef __TEST_WAV_SOUND_HPP__
#define __TEST_WAV_SOUND_HPP__

#include "test_common.hpp"
#include "sound/wav_sound.hpp"

TEST(TestWav, base_test)
{
	mhe::OpenALAudioDriver driver;
	ASSERT_EQ(true, driver.init());
	mhe::ResourceManager<mhe::SoundLoader> loader;
	boost::shared_ptr<mhe::iSound> sound = loader.get("assets/main_menu/button-3.wav");
	ASSERT_NE(sound, boost::shared_ptr<mhe::iSound>());
	sound->play();
	mhe::test::sleep(3000);
	sound->stop();
}

#endif
