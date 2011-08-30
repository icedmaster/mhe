#ifndef __TEST_COMMON_HPP__
#define __TEST_COMMON_HPP__

#include <gtest/gtest.h>

#ifdef __WIN32__
	#include <windows.h>
	#include <process.h>
#else
	#include <unistd.h>
#endif	// win32

#include "mhe.hpp"
#include "test_nodes.hpp"

namespace mhe
{
namespace test
{
	void sleep(unsigned int msec)
	{
		#ifdef __WIN32__
		Sleep(msec);
		#else
		usleep(msec * 1000);
		#endif
	}

	class TestCommon : public ::testing::Test
	{
		typedef PrivateEventListener<TestCommon> TestEventListener;
		friend class PrivateEventListener<TestCommon>; 

		bool stop(const Event&)
		{
			running = false;
			return true;
		}

		protected:
			WindowSystem window_system;
			InputSystem input_system;
			TextureManager texture_manager;
			boost::shared_ptr<iDriver> driver;
			bool running;
		protected:
			virtual void SetUp()
			{
				window_system.init(800, 600, 32);
				input_system.setWindowSystem(&window_system);
				input_system.addListener(new TestEventListener(SystemEventType, QUIT, 0,
															   this, &TestCommon::stop));
				driver.reset(SystemFactory::instance().createDriver());
				driver->set_window_system(&window_system);
				window_system.showCursor(true);
		
				driver->set_clear_color(cfBlack);
				driver->enable_depth((DepthFunc)0);
				driver->disable_lighting();

				matrixf om;
				om.set_ortho(0, 800, 0, 600, -1, 1);
				driver->set_projection_matrix(om);
		
				running = true;
			} 			
	};	
}
}

#endif
