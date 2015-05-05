#include "platform/win/win_fs.hpp"

#include "platform/win/win_wrapper.hpp"

namespace mhe {
namespace winsys {

bool is_path_exists(const FilePath& fullpath)
{
	DWORD attr = GetFileAttributes(fullpath.c_str());

	return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool mkdir(const FilePath& fullpath)
{
	return ::CreateDirectory(fullpath.c_str(), 0) == TRUE;
}

FilePath get_user_temp_dir()
{
	FilePath ret;
	DWORD len = ::GetTempPath(ret.capacity(), ret.buffer());
	ret.resize(len);
	return ret;
}

}}
