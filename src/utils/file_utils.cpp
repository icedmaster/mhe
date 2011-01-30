#include "utils/file_utils.hpp"
#include <algorithm>
#include <cctype>

namespace mhe
{
    std::string get_file_extensiton(const std::string& filename)
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
};
