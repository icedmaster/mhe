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
		boost::shared_ptr<log> createStandartLog();

		// write to singleton log functions
		void log_write(const std::string& s);

		// Singleton class for easy logging in project
		class global_log : public mixlog
		{
			private:
				global_log() {}
				global_log(const global_log&) {}
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
