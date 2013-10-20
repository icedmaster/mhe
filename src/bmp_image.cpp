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

	bool res = load_impl(f);

	f.close();
	return res;
}

bool bmp_image::load_impl(std::istream& stream)
{
    // As we works with stream position - save it first, because
    // we can get modified stream at function input
    size_t start_pos = stream.tellg();
    stream.seekg(0, std::ios::end);
	size_t file_len = stream.tellg();
	file_len -= start_pos;
	stream.seekg(start_pos);

	// read data to vector
	std::vector<char> filedata;
	filedata.resize(file_len);
	stream.read(&filedata[0], file_len);

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

bool bmp_image::save_impl(const std::string& fn)
{
	std::ofstream f(fn.c_str(), std::ios::binary);
	if (!f.is_open()) return false;

	bool res = save_impl(f);
	f.close();
	return res;
}

bool bmp_image::save_impl(std::ostream& stream)
{
    // create copy of data
	std::vector<char> d;
	d.reserve(data.size());
	//d.assign(data.begin(), data.end());
	// swap R and B component before writing
	//swapRB(fi.bit_count, data, d);
	for (size_t i = 0; i < data.size(); i += 3)
	{
	    d.push_back(data[i + 2]);
	    d.push_back(data[i + 1]);
	    d.push_back(data[i]);
	    if (fi.bit_count == 32)
            ++i;
	}

	cmn::uint bpp = (fi.bit_count == 32) ? 24 : fi.bit_count;

	// write file header
	bmp_file_header bfh;
	bfh.type = 0x4d42;  // BM
	bfh.offs = file_header_size + file_info_size;
	bfh.size = bfh.offs + fi.width * fi.height * (bpp >> 3);
	bfh.res1 = bfh.res2 = 0;

	stream.write((char*)&(bfh.type), sizeof(bfh.type));
	stream.write((char*)&(bfh.size), sizeof(bfh.size));
	stream.write((char*)&(bfh.res1), sizeof(bfh.res1));
	stream.write((char*)&(bfh.res2), sizeof(bfh.res2));
	stream.write((char*)&(bfh.offs), sizeof(bfh.offs));

	//f.write(fh, file_header_size);

	// write file info
	char finfo[file_info_size];
	memset(finfo, 0, file_info_size);
	memcpy(finfo, &(file_info_size), 4);
	memcpy(finfo + 4, &(fi.width), 4);
	memcpy(finfo + 8, &(fi.height), 4);

	cmn::uint16_t planes_count = 1;
	memcpy(finfo + 12, &planes_count, 2);
	memcpy(finfo + 14, &(bpp), 2);
	stream.write(finfo, file_info_size);

	// write file data
	stream.write(&d[0], d.size());

	return true;
}
