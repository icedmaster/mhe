#ifndef _ITEXTURE_HPP_
#define _ITEXTURE_HPP_

#include "image.hpp"
#include <map>
#include <boost/shared_ptr.hpp>

namespace mhe
{
	enum FilterType
	{
        Nearest
	};

	// do we need to store image in texture
	enum StoreType
	{
		NotStore,
		Store
	};

	class iTexture
	{
		private:
			virtual void set_image(const boost::shared_ptr<Image>&, StoreType, FilterType) = 0;	// load image data to videobuffer
			virtual void clear_impl() = 0;
			virtual void set_u_impl(float) = 0;
			virtual void set_v_impl(float) = 0;
			virtual float get_u() const = 0;
			virtual float get_v() const = 0;
			virtual void set_filter(FilterType) = 0;
			virtual cmn::uint get_id() const = 0;
			virtual void set_name(const std::string&) = 0;
			virtual const std::string& get_name() const = 0;
		public:
			void setImage(const boost::shared_ptr<Image>& im, StoreType st = NotStore, FilterType ft = Nearest)
			{
				set_image(im, st, ft);
			}

			void clear()
			{
				clear_impl();
			}

			void set_u(float u)
			{
				set_u_impl(u);
			}

			void set_v(float v)
			{
				set_v_impl(v);
			}

			float u() const
			{
				return get_u();
			}

			float v() const
			{
				return get_v();
			}

			void setFilter(FilterType ft)
			{
				set_filter(ft);
			}

			cmn::uint id() const
			{
				return get_id();
			}

			void setName(const std::string& name)
			{
				set_name(name);
			}

			const std::string& getName() const
			{
				return get_name();
			}
	};

    /*
	class TextureManager
	{
		private:
			typedef std::map< cmn::uint, boost::shared_ptr<iTexture> > texmap;
			texmap mTextures;
		public:
			void add(const boost::shared_ptr<iTexture>& t)
			{
				mTextures.insert(texmap::value_type(t->id(), t));
			}

			void add(const iTexture* t)
			{
				boost::shared_ptr<iTexture> tt(t);
				mTextures.insert(texmap::value_type(tt->id(), t));
			}

			const boost::shared_ptr<iTexture>& get(cmn::uint id) const
			{
				return mTextures[id];
			}

			const boost::shared_ptr<iTexture>& get(const std::string& name) const
			{
				for (texmap::const_iterator it = mTextures.begin();
											it != mTextures.end();
											++it)
				{
					texmap::value_type v = *it;
					if (v.second->getName() == name)
						return v.second;
				}

				throw mhe::exception("texture not found");
			}
	};
	*/
};

#endif
