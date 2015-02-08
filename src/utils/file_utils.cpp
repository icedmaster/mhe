#include "utils/file_utils.hpp"
#include <algorithm>
#include <cctype>

namespace mhe {
namespace utils {

std::string get_file_extension(const std::string& filename)
{
    return std::string(get_file_extension(FilePath(filename.c_str())).c_str());
}

FilePath get_file_extension(const FilePath& filename)
{
	size_t point_pos = filename.find_last_of('.');
	if (point_pos == std::string::npos)
		return "";
    FilePath ext = filename.substr(point_pos + 1, filename.length() - point_pos - 1);
	return ext;
}

std::string get_file_name(const std::string& filepath)
{
	size_t slash_pos = filepath.find_last_of('/');
	// get filename with extension
	std::string fne;
	if (slash_pos == std::string::npos)
		fne = filepath;
	else
		fne = filepath.substr(slash_pos + 1, filepath.length() - slash_pos - 1);
	// remove extension
	size_t point_pos = fne.find_last_of('.');
	std::string fn = fne;
	if (point_pos != std::string::npos)
		fn.erase(point_pos, fn.length() - point_pos);
	return fn;
}

std::string get_file_name_with_path(const std::string& filepath)
{
	size_t point_pos = filepath.find_last_of('.');
	if (point_pos == std::string::npos) return filepath;
	return filepath.substr(0, point_pos);
}

FilePath get_file_path(const FilePath& fullpath)
{
	size_t slash_pos = fullpath.find_last_of('/');
    if (slash_pos == FilePath::npos) return fullpath;
	return fullpath.substr(0, slash_pos + 1);
}

std::string get_file_path(const std::string &fullpath)
{
    return std::string(get_file_path(FilePath(fullpath.c_str())).c_str());
}

std::string read_whole_file(std::ifstream& stream)
{
	return std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
}

std::vector<std::string> read_lines(std::ifstream& stream)
{
	std::vector<std::string> result;
	while (!stream.eof())
	{
		std::string line;
		std::getline(stream, line);
		result.push_back(line);
	}
	return result;
}

}}
