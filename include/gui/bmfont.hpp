#ifndef __BMFONT_HPP__
#define __BMFONT_HPP__

#include <map>
#include "font.hpp"
#include "texture_atlas.hpp"

namespace mhe {
namespace gui {

class 
BMFont : public Font
{
public:
	bool load(const std::string& filename);
	void print(const boost::shared_ptr<Driver>& driver, const std::wstring& text,
			   const vector2<int>& coord, const colorf& color = color_white);
	std::string name() const
	{
		return name_;
	}

	cmn::uint height() const
	{
		return height_;
	}
private:
	void load_char(const std::string& str,
				   std::vector<float>& texc);
	void load_texture(const std::string& filename);
	cmn::uint get_char(cmn::uint sym) const;

	TextureAtlas ta_;
	std::map<cmn::uint, cmn::uint> chars_;
	std::string name_;
	cmn::uint height_;
};

}
}

#endif
