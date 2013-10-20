#ifndef _OGG_SOUND_HPP_
#define _OGG_SOUND_HPP_

#include "../isound_data.hpp"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <vector>

namespace mhe
{	
	class ogg_sound : public iSoundData
	{						
		private:
			std::vector<cmn::int16_t> data_;
			SoundParams sp_;
		public:
			bool load(const std::string& filename);
			bool load(std::istream& stream);

			const std::vector<cmn::int16_t>& get() const
			{
				return data_;
			}

			SoundFormat format() const
			{
				return sp_.format;
			}
			
			cmn::uint freq() const
			{
				return sp_.freq;
			}
	};
}

#endif
