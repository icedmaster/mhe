#ifndef _FILE_UTILS_HPP_
#define _FILE_UTILS_HPP_

#include <string>

namespace mhe
{
    std::string get_file_extensiton(const std::string& filename);
	std::string get_file_name(const std::string& filepath);
};

#endif
