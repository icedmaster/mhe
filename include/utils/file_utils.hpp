#ifndef _FILE_UTILS_HPP_
#define _FILE_UTILS_HPP_

#include <string>
#include <fstream>
#include "core/compiler.hpp"

namespace mhe {
namespace utils {

MHE_EXPORT std::string get_file_extension(const std::string& filename);
MHE_EXPORT std::string get_file_name(const std::string& filepath);
MHE_EXPORT std::string get_file_name_with_path(const std::string& filepath);
MHE_EXPORT std::string get_file_path(const std::string& fullpath);
MHE_EXPORT std::string read_whole_file(std::ifstream& stream);

// path utils
inline std::string path_join(const std::string& path1, const std::string& path2)
{
	return path1 + path2;
}
    
}}

#endif
