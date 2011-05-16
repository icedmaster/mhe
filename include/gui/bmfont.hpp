#ifndef _BMFONT_HPP_
#define _BMFONT_HPP_

#include "ifont.hpp"
#include "texture_atlas.hpp"
#include <map>

namespace mhe
{
	namespace gui
	{
		class BMFont : public iFont
		{
			private:
                TextureAtlas ta_;
				cmn::uint height_;
				std::string name_;
				colorf color_;
				std::map<cmn::uint, cmn::uint> chars_;

				void load_char(const std::string& str,
							   std::vector<float>& texc);
				void load_texture(const std::string& filename);
				cmn::uint get_char(cmn::uint sym) const;
			public:
				bool load(const std::string& filename);

				void print(const boost::shared_ptr<iDriver>& driver, const std::wstring& text,
						   const v2d& coord);
				void setColor(const colorf& color)
				{
					color_ = color;
				}

				cmn::uint height() const
				{
					return height_;
				}

				std::string name() const
				{
					return name_;
				}

				iFont* clone() const;
		};
	}
}

#endif
