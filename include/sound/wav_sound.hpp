#ifndef __WAV_SOUND_HPP__
#define __WAV_SOUND_HPP__

#include "isound_data.hpp"
#include "wav_struct.hpp"

namespace mhe
{
class wav_sound : public iSoundData
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
