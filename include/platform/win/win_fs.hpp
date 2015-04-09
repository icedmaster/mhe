#ifndef __MHE_FS_HPP__
#define __MHE_FS_HPP__

#include "core/string.hpp"

namespace mhe {
namespace winsys {

bool is_path_exists(const FilePath& fullpath);
bool mkdir(const FilePath& fullpath);
FilePath get_user_temp_dir();

}}

#endif
