#ifndef _FILE_UTILS_HPP_
#define _FILE_UTILS_HPP_

#include <string>
#include <fstream>

namespace mhe {
namespace utils {

std::string get_file_extension(const std::string& filename);
std::string get_file_name(const std::string& filepath);
std::string get_file_name_with_path(const std::string& filepath);
std::string read_whole_file(std::ifstream& stream);
    
}}

#endif
