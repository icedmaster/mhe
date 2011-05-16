#ifndef _FILESYSTEM_HPP_
#define _FILESYSTEM_HPP_

#include <string>
#include "file_utils.hpp"

namespace mhe
{
	class path
	{
		private:
			std::wstring filename_;
			std::wstring filepath_;
		public:
			path()
			{}
			
			path(const std::wstring& fullpath) :
				filename_(get_filename(fullpath)),
				filepath_(get_filepath(fullpath))
			{}
			
			path(const std::wstring& fpath, const std::wstring& fname) :
				filename_(fname),
				filepath_(fpath)
			{}				
			
			const std::wstring& filename() const
			{
				return filename_;
			}
			
			void set_filename(const std::wstring& fname)
			{
				filename_ = fname;
			}
			
			const std::wstring& filepath() const
			{
				return filepath_;
			}
			
			void set_filepath(const std::wstring& fpath)
			{
				filepath_ = fpath;
			}
			
			std::wstring fullpath() const
			{
				return filepath_ + filename_;
			}
			
			void set_fullpath(const std::wstring& fullpath)
			{
				filename_ = get_filename(fullpath);
				filepath_ = get_filepath(fullpath);
			}
			
			bool empty() const
			{
				return (filename_.empty() && filepath_.empty());
			}
			
			bool is_file() const
			{
				return !filename_.empty();
			}
			
			bool is_directory() const
			{
				return (!filepath_.empty() && filename_.empty());
			}
	};

	class filesystem
	{
		private:
			path cwd_;
		public:
			filesystem();
			filesystem(const path& wd);
			
			void change_wd(const path& p);
			const path& cwd() const
			{
				return cwd_;
			}
			
			std::wstring get_path(const std::wstring& filename) const;
	};
}

#endif
