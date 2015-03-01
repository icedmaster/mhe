#ifndef _FILE_UTILS_HPP_
#define _FILE_UTILS_HPP_

#include <string>
#include <fstream>
#include <vector>
#include "core/compiler.hpp"
#include "core/string.hpp"

namespace mhe {
namespace utils {

MHE_EXPORT std::string get_file_extension(const std::string& filename);
MHE_EXPORT std::string get_file_name(const std::string& filepath);
MHE_EXPORT std::string get_file_name_with_path(const std::string& filepath);
MHE_EXPORT std::string get_file_path(const std::string& fullpath);
MHE_EXPORT std::string read_whole_file(std::ifstream& stream);
MHE_EXPORT std::vector<std::string> read_lines(std::ifstream& stream);

MHE_EXPORT FilePath get_file_extension(const FilePath& filename);
MHE_EXPORT FilePath get_file_path(const FilePath& fullpath);
MHE_EXPORT FilePath get_file_name(const FilePath& fullpath);
MHE_EXPORT FilePath get_file_name_with_extension(const FilePath& fullpath);
MHE_EXPORT FilePath convert_slashes(const FilePath& fullpath);

// path utils
inline std::string path_join(const std::string& path1, const std::string& path2)
{
	return path1 + path2;
}

template <size_t N1, size_t N2>
inline FilePath path_join(const basic_string<char, N1>& path1, const basic_string<char, N2>& path2)
{
    FilePath path(path1);
    return path + path2;
}
    
}}

#endif
