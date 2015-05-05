#ifndef __PLATFORM_FS_HPP__
#define __PLATFORM_FS_HPP__

#include "core/string.hpp"
#include "core/config.hpp"

#ifdef MHE_WIN
#include "win/win_fs.hpp"
#endif

namespace mhe {
namespace impl {

inline bool is_path_exists(const FilePath& fullpath)
{
#ifdef MHE_WIN
	return winsys::is_path_exists(fullpath);
#endif
}

inline bool mkdir(const FilePath& fullpath)
{
#ifdef MHE_WIN
	return winsys::mkdir(fullpath);
#endif
}

inline FilePath get_user_temp_dir()
{
#ifdef MHE_WIN
	return winsys::get_user_temp_dir();
#endif
}

}}

#endif
