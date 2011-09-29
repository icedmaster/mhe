#include "sound/wav_sound.hpp"
#include "utils/numutils.hpp"
#include <fstream>

#include <iostream>

namespace mhe {

bool wav_sound::load(const std::string& filename)
{
	std::ifstream f(filename.c_str(), std::ios::binary);
	if (!f.is_open()) return false;
	bool res = load(f);
	f.close();
	return res;
}

bool wav_sound::load(std::istream& stream)
{
	riff_header rh;
	// load first header
	stream.read(reinterpret_cast<char*>(&rh), sizeof(rh));
	if (utils::char_to_int_le(rh.id) != riff_header_id)
		return false;
	// read second header
	wav_chuck_header ch;
	stream.read(reinterpret_cast<char*>(&ch), sizeof(ch));
	if ( (utils::char_to_int_le(ch.id) != wav_chuck_header_id) ||
		 (utils::char_to_int_le(ch.fmt) != wav_chuck_header_fmt) )
		return false;
	// read third header
	wav_header wh;
	stream.read(reinterpret_cast<char*>(&wh.type), sizeof(wh.type));
	if (wh.type != 1) return false;	// not supperted
	stream.read(reinterpret_cast<char*>(&wh.channels), sizeof(wh.channels));
	sp_.format = (wh.channels == 1) ? Mono : Stereo;
	stream.read(reinterpret_cast<char*>(&wh.samples_per_second),
				sizeof(wh.samples_per_second));
	stream.read(reinterpret_cast<char*>(&wh.bytes_per_second),
				sizeof(wh.bytes_per_second));
	stream.read(reinterpret_cast<char*>(&wh.align), sizeof(wh.align));
	stream.read(reinterpret_cast<char*>(&wh.bits_per_sample),
				sizeof(wh.bits_per_sample));
	wh.extra_format_bytes = 0;
	if (wh.type != 1)
		stream.read(reinterpret_cast<char*>(&wh.extra_format_bytes),
					sizeof(wh.extra_format_bytes));
	if (wh.extra_format_bytes)
	{
		std::vector<char> eb(wh.extra_format_bytes);
		stream.read(&eb[0], wh.extra_format_bytes);
	}
	sp_.freq = wh.samples_per_second;
	// read forth header
	wav_sample_header sh;
	stream.read(reinterpret_cast<char*>(&sh), sizeof(sh));
	if (utils::char_to_int_le(sh.id) != wav_sample_header_id)
		return false;
	int num_samples = sh.len / (wh.bits_per_sample / 8);
	data_.resize(num_samples);
	if (wh.bits_per_sample == 16)
		stream.read(reinterpret_cast<char*>(&data_[0]), sh.len);
	else	// 8bit obly
	{
		std::cout << "8bit " << num_samples << std::endl;
		std::vector<char> tmp(sh.len);
		stream.read(&tmp[0], sh.len);
		//data_.assign(tmp.begin(), tmp.end());
		for (size_t i = 0; i < tmp.size(); ++i)
			data_[i] = (tmp[i] - 128) << 4;
	}
	if (static_cast<cmn::uint32_t>(stream.gcount()) != sh.len) return false;
	return true;
}

}	// namespace 
