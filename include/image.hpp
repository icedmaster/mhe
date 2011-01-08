#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include "types.hpp"
#include <vector>
#include <string>

namespace mhe
{
	class Image
	{		
		protected:			
			virtual bool load_impl(const std::string& fn) = 0;
			virtual const std::vector<char>& get_impl() const = 0;
			virtual void set_impl(const std::vector<char>&, cmn::uint, cmn::uint) = 0;
			virtual cmn::uint get_width() const = 0;
			virtual cmn::uint get_height() const = 0;
			virtual cmn::uint get_bpp() const = 0;
		public:						
			virtual ~Image()
			{}
			
			bool load(const std::string& fn)
			{
				return load_impl(fn);
			}
			
			cmn::uint width() const
			{
				return get_width();
			}
			
			cmn::uint height() const
			{
				return get_height();
			}
			
			cmn::uint bpp() const
			{
				return get_bpp();
			}
			
			const std::vector<char>& get() const
			{
				return get_impl();
			}
			
			void set(const std::vector<char>& d, cmn::uint w, cmn::uint h)
			{
				set_impl(d, w, h);
			}
	};

	void swapRB(Image& im);	
	void addAlphaChannel(Image& im, char a);
};

#endif
