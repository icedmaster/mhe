#include "bmp_image.hpp"
#include <fstream>

#define __debug__

#ifdef __debug__
	#include <iostream>
#endif

using namespace mhe;

bool bmp_image::correct_header(const std::vector<char>& h)
{
	if (h.size() != file_header_size) return false;
	if ( (h[0] != 'B') || (h[1] != 'M') )
		return false;
	return true;
}


bool bmp_image::load_impl(const std::string& fn)
{
	#ifndef __WIN32__
	std::ifstream f(fn.c_str());
	#else
	std::ifstream f(fn.c_str(), std::ios::binary);
	#endif
	if (!f.is_open()) return false;

	f.seekg(0, std::ios::end);
	size_t file_len = f.tellg();
	f.seekg(0, std::ios::beg);

	// read data to vector
	std::vector<char> filedata;
	filedata.resize(file_len);
	f.read(&filedata[0], file_len);
	f.close();

	if ( file_len < (file_header_size + file_info_size) )
		return false;

	// read file header
	std::vector<char> fh;
	fh.resize(file_header_size);
	fh.assign(filedata.begin(), filedata.begin() + file_header_size);

	if (!correct_header(fh)) return false;

	filedata.erase(filedata.begin(), filedata.begin() + file_header_size);
	// read file info
	std::vector<char> fiv;
	fiv.assign(filedata.begin(), filedata.begin() + file_info_size);
	// get parameters
	// TODO:
	memcpy((char*)&(fi.width), &fiv[4], 4);
	memcpy((char*)&(fi.height), &fiv[8], 4);
	memcpy((char*)&(fi.bit_count), &fiv[14], 2);

	#ifdef __debug__
	std::cout << "bpp: " << fi.bit_count << " w: " << fi.width << " h: " << fi.height << std::endl;
	#endif
	//
	if (fi.bit_count < 24) return false;

	const int bpp = fi.bit_count >> 3;
	// read data
	data.resize(fi.width * fi.height * bpp);
	filedata.erase(filedata.begin(), filedata.begin() + file_info_size);
	data.assign(filedata.begin(), filedata.end());
	if (data.size() != (fi.width * fi.height * bpp))
		return false;

    swapRB(*this);

	return true;
}
