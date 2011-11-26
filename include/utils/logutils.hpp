//
//	Different utilities for logging system
//
#ifndef _LOGUTILS_HPP_
#define _LOGUTILS_HPP_

#include "netlog.hpp"

namespace mhe
{
	namespace utils
	{
		// Create std|file|net log
		bool createStandartLog();

		// Singleton class for easy logging in project
		class global_log : public mixlog
		{
			private:
				global_log() {}
				~global_log() {}
			public:
				static global_log& instance()
				{
					static global_log log;
					return log;
				}
		};

	}
};

#endif
