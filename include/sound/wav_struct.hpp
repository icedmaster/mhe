#ifndef __WAV_STRUCT_HPP__
#define __WAV_STRUCT_HPP__

#include "types.hpp"

namespace mhe
{
const cmn::uint32_t riff_header_id = 0x46464952;

struct riff_header
{
	char id[4];	// must be 0x46464952
	cmn::uint32_t len;
};

const cmn::uint32_t wav_chuck_header_id = 0x45564157;
const cmn::uint32_t wav_chuck_header_fmt = 0x20746d66;

struct wav_chuck_header
{
	char id[4];	// must be 0x45564157
	char fmt[4];	// must be 0x20746D66
	cmn::uint32_t len;
};

struct wav_header
{
	cmn::uint16_t type;
	cmn::uint16_t channels;
	cmn::uint32_t samples_per_second;
	cmn::uint32_t bytes_per_second;
	cmn::uint16_t align;
	cmn::uint16_t bits_per_sample;
	cmn::uint16_t extra_format_bytes;
};

const cmn::uint32_t wav_sample_header_id = 0x61746164;

struct wav_sample_header
{
	char id[4];	// must be 0x61746164
	cmn::uint32_t len;
};

}

#endif
