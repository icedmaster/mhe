#ifndef _ISOUND_DATA_HPP_
#define _ISOUND_DATA_HPP_

#include "types.hpp"
#include <string>
#include <vector>

namespace mhe
{
	enum SoundFormat
	{
		Mono,
		Stereo
	};

	struct SoundParams
	{
		SoundFormat format;
		cmn::uint samples_count;		
		cmn::uint freq;
	};
	
	class iSoundData
	{
		public:
			virtual ~iSoundData() {}
			virtual bool load(const std::string& filename) = 0;
			virtual bool load(std::istream& stream) = 0;
			virtual const std::vector<cmn::int16_t>& get() const = 0;
			virtual SoundFormat format() const = 0;
			virtual cmn::uint freq() const = 0;
	};
}

#endif
