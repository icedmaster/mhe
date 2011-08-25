#ifndef __TEST_COMMON_HPP__
#define __TEST_COMMON_HPP__

#include <gtest/gtest.h>

#ifdef __WIN32__
	#include <windows.h>
	#include <process.h>
#else
	#include <unistd.h>
#endif	// win32

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
}
}

#endif
