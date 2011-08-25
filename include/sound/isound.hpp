#ifndef _ISOUND_HPP_
#define _ISOUND_HPP_

#include "isound_data.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class iSound
	{
		public:
			virtual ~iSound() {}
			
			virtual bool init(boost::shared_ptr<iSoundData> data) = 0;
			
			virtual void play() = 0;
			virtual void stop() = 0;
			virtual bool is_playing() const = 0;
	};
}

#endif
