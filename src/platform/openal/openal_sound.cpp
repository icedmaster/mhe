#include "platform/openal/openal_sound.hpp"

namespace mhe
{
	OpenALSound::OpenALSound() :
		id_(0xffffffff),
		src_id_(0xffffffff)
	{}
	
	OpenALSound::~OpenALSound()
	{
		if (src_id_ != 0xffffffff)
			alDeleteSources(1, &src_id_);
		if (id_ != 0xffffffff)
			alDeleteBuffers(1, &id_);
	}
	
	bool OpenALSound::init(boost::shared_ptr<iSoundData> data)
	{
		// prepare buffers
		ALenum format = (data->format() == Mono) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		alGenBuffers(1, &id_);
		alBufferData(id_, format, &(data->get()[0]), data->get().size() * 2, data->freq());
		
		// prepare source
		alGenSources(1, &src_id_);
		alSourcei(src_id_, AL_BUFFER, id_);
		alSourcef(src_id_, AL_MIN_GAIN, 0.0f);
		alSourcef(src_id_, AL_MAX_GAIN, 1.0f);
		
		return true;
	}
	
	void OpenALSound::play()
	{
		alSourcePlay(src_id_);
	}
	
	void OpenALSound::stop()
	{
		alSourceStop(src_id_);
	}

	bool OpenALSound::is_playing() const
	{
		int value;
		alGetSourcei(src_id_, AL_SOURCE_STATE, &value);
		return (value == AL_PLAYING);
	}
}
