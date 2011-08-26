#ifndef _IAUDIO_DRIVER_HPP_
#define _IAUDIO_DRIVER_HPP_

namespace mhe
{
	class iAudioDriver
	{
		public:
			virtual ~iAudioDriver() {}
			virtual bool init() = 0;
	};
}

#endif
