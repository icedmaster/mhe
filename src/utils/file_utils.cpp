#include "utils/file_utils.hpp"
#include <algorithm>
#include <cctype>

namespace mhe
{
    std::string get_file_extension(const std::string& filename)
    {
        size_t point_pos = filename.find_last_of('.');
        if (point_pos == std::string::npos)
            return "";
        std::string ext = filename.substr(point_pos + 1, filename.length() - point_pos - 1);
        // FIXME: I want to use STL algorithm
        // std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower)
        // but have error with GCC compiler
        for (size_t i = 0; i < ext.length(); ++i)
            ext[i] = std::tolower(ext[i]);
        return ext;
    }

	std::string get_file_name(const std::string& filepath)
	{
		size_t slash_pos = filepath.find_last_of('/');
		// get filename with extension
		const std::string& fne = filepath.substr(slash_pos + 1, filepath.length() - slash_pos - 1);
		// remove extension
		size_t point_pos = fne.find_last_of('.');
		std::string fn = fne;
		if (point_pos)
			fn.erase(point_pos, fn.length() - point_pos);
		return fn;
	}
};
