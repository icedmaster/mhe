#ifndef __TEST_SOUND_HPP__
#define __TEST_SOUND_HPP__

#include "test_common.hpp"
#include "mhe_sound.hpp"
#include "resource_manager.hpp"
#include "resource_loader.hpp"

TEST(TestSound, test)
{
	// open sound system
	mhe::OpenALAudioDriver driver;
	ASSERT_EQ(true, driver.init());

	mhe::ResourceManager<mhe::SoundLoader> loader;
	boost::shared_ptr<mhe::iSound> sound = loader.get("assets/test1.ogg");
	boost::shared_ptr<mhe::iSound> sound2 = loader.get("assets/test.ogg");
	ASSERT_NE(sound, boost::shared_ptr<mhe::iSound>());
	ASSERT_NE(sound2, boost::shared_ptr<mhe::iSound>());
	sound2->play();
	sound->play();
	EXPECT_EQ(true, sound->is_playing());
	EXPECT_EQ(true, sound2->is_playing());
	mhe::test::sleep(7000);
	EXPECT_FALSE(sound->is_playing() == true);
	mhe::test::sleep(3000);
	sound->stop();
	sound2->stop();

/*
	// load sound data
	boost::shared_ptr<mhe::ogg_sound> data(new mhe::ogg_sound());
	ASSERT_EQ(true, data->load("assets/test2.ogg"));
	
	// init sound
	mhe::OpenALSound sound;
	ASSERT_EQ(true, sound.init(data));
	sound.play();
	
//	mhe::test::sleep(5000);
	Sleep(5000);
	sound.stop();
*/
}

TEST(TestSound, longplay)
{
	mhe::OpenALAudioDriver driver;
	ASSERT_EQ(true, driver.init());

	mhe::ResourceManager<mhe::SoundLoader> loader;
	boost::shared_ptr<mhe::iSound> sound = loader.get("assets/main_menu/theme.ogg");
	sound->play();
	for (cmn::uint i = 0; i < 6; ++i)
	{
		ASSERT_TRUE(sound->is_playing());
		mhe::test::sleep(10000);
	}
	sound->stop();
}

#endif
