#ifndef _GL_STATE_ATTR_HPP_
#define _GL_STATE_ATTR_HPP_

#include "mhe_gl.hpp"
#include "types.hpp"
#include <set>

namespace mhe
{
	class GLStateAttr
	{
		private:
			std::set<cmn::uint> attr_;
			std::set<cmn::uint> attr_copy_;
			bool saved_;
			GLStateAttr() : saved_(false)
			{}
			~GLStateAttr() {}
			GLStateAttr(const GLStateAttr&) {}
			void operator= (const GLStateAttr&) {}
		public:
			static GLStateAttr& instance()
			{
				static GLStateAttr ga;
				return ga;
			}
			
			void set(cmn::uint attr)
			{
				if (attr_.find(attr) != attr_.end())
				{
					attr_.insert(attr);
					glEnable(attr);
				}
			}
			
			void clr(cmn::uint attr)
			{
				if (attr_.find(attr) != attr_.end())
				{
					attr_.erase(attr);
					glDisable(attr);
				}
			}
			
			bool get(cmn::uint attr)
			{
				if (attr_.find(attr) != attr_.end())
					return true;
				return false;
			}
			
			void save()
			{
				attr_copy_ = attr_;
				saved_ = true;
			}
			
			void restore()
			{
				if (saved_)
				{
					attr_ = attr_copy_;
					for (std::set<cmn::uint>::const_iterator it = attr_.begin();
															 it != attr_.end();
															 ++it)
						glEnable(*it);
				}
				saved_ = false;
			}			
	};
};

#endif
			