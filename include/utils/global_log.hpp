#ifndef _GLOBAL_LOG_HPP_
#define _GLOBAL_LOG_HPP_

#include "log.hpp"

namespace mhe
{
	class global_log : public mixlog
	{
		private:
			global_log() {}
			~global_log() {}			
		public:
			const global_log& instance() const
			{
				static global_log log;
				return log;
			}					
	};
	
};

#endif
