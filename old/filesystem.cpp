#include "filesystem.hpp"

namespace mhe
{
	filesystem::filesystem() :
		cwd_(".\")
	{}
	
	filesystem::filesystem(const path& wd) :
		cwd_(wd.filepath())
	{}
			
	void filesystem::change_wd(const path& p)
	{
		cwd_ = p.filepath();
	}
	
	std::wstring filesystem::get_path(const std::wstring& filename) const
	{
		path p(cwd_, filename);
		return p.fullpath();
	}
}
