#include "sound/ogg_sound.hpp"
#include <fstream>
#include <cstring>

namespace
{
	size_t ogg_read(void* ptr, size_t size, size_t ne, void* source);
	int ogg_seek(void* source, ogg_int64_t offset, int whence);
	int ogg_close(void* source);
	long ogg_tell(void* source);
}
	

namespace mhe
{
	bool ogg_sound::load(const std::string& filename)
	{
		std::ifstream f(filename.c_str(), std::ios::binary);
		if (!f.is_open())
			return false;
		bool res = load(f);
		f.close();
		return res;
	}

	bool ogg_sound::load(std::istream& stream)
	{		
		OggVorbis_File f;
		memset((char*)&f, 0, sizeof(f));
		ov_callbacks ov_cb;
		ov_cb.read_func  = ogg_read;
		ov_cb.seek_func  = ogg_seek;
		ov_cb.tell_func  = ogg_tell;
		ov_cb.close_func = ogg_close;
		ov_open_callbacks(&stream, &f, 0, 0, ov_cb);
		
		vorbis_info* vi = ov_info(&vf, -1);
		int num_channels = vi->channels;
		if (num_channels == 1)
			sp_.format = Mono;
		else if (num_channels == 2)
			sp_.format = Stereo;
		sp_.freq = vi->rate;
		int num_samples = ov_pcm_total(&vf, -1);
		int total_size = num_samples * num_channels;
		data_.resize(total_size);
		// size to read - in bytes
		total_size *= sizeof(cmn::int16_t);

		// last - read all samples
		int rd_sz = 0;		
		char* ptr = reinterpret_cast<char*>(&data_[0]);
		while (rd_sz < total_size)
		{
			int bitstream;			
			int rd = ov_read(&fv, ptr + rd_sz, total_size - rd_sz, 0, 2, 1, &bitstream);
			if (rd <= 0)
				break;
			rd_sz += rd;
		}
		ov_clear(&vf);
		
		return true;
	}
}

namespace
{
	size_t ogg_read(void* ptr, size_t size, size_t ne, void* source)
	{
		std::istream* stream = static_cast<std::istream*>(source);
		stream->read(static_cast<char*>(ptr), size * ne);
		return stream->tellg();
	}
	
	int ogg_seek(void* source, ogg_int64_t offset, int whence)
	{
		std::istream* stream = static_cast<std::istream*>(source);
		std::seekdir dir;
		switch (whence) 
		{
			case SEEK_SET: dir = ios::beg; break;
			case SEEK_CUR: dir = ios::cur; break;
			case SEEK_END: dir = ios::end; break;
			default: return -1;
		}
		stream->seekg((std::streamoff)offset, dir);
		return (stream->fail() ? 1 : 0);	
	}

	int ogg_close(void* source)
	{
		return 0;
	}

	long ogg_tell(void* source)
	{
		return static_cast<std::istream*>(source)->tellg();
	}
}
