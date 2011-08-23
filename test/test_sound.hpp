#ifndef __TEST_SOUND_HPP__
#define __TEST_SOUND_HPP__

#include "test_common.hpp"
#include "mhe_sound.hpp"

#ifndef __WIN32__
#include <unistd.h>
#endif

TEST(TestSound, test)
{
	// open sound system
	mhe::OpenALAudioDriver driver;
	ASSERT_EQ(true, driver.init());
	// load sound data
	boost::shared_ptr<mhe::ogg_sound> data(new mhe::ogg_sound());
	ASSERT_EQ(true, data->load("assets/test1.ogg"));
	
	// init sound
	mhe::OpenALSound sound;
	ASSERT_EQ(true, sound.init(data));
	sound.play();
	
//	mhe::test::sleep(5000);
	#ifdef __WIN32__
	Sleep(5000);
	#else
	sleep(5);
	#endif
	sound.stop();
}

#endif
