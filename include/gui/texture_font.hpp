#ifndef __TEXTURE_FONT_HPP__
#define __TEXTURE_FONT_HPP__

#include "font.hpp"
#include "texture_atlas.hpp"

namespace mhe {
namespace gui {

class TextureFont : public Font
{
public:
	virtual ~TextureFont() {}

	bool load(const std::string& filename);

	Renderable* print(const utf32_string& text,
            const vector2<float>& position, const colorf& color = color_white);

	std::string name() const
	{
		return name_;
	}

	cmn::uint height() const
	{
		return height_;
	} 
protected:
	void set_name(const std::string& name)
	{
		name_ = name;
	}

	void set_height(cmn::uint height)
	{
		height_ = height;
	}

	void load_texture(const std::string& filename);
	void add_char_for_index(utf32_char c, const rect<float>& rect);	
private:
	virtual bool load_font_parameters(std::istream& stream, std::string& texture_filename) = 0;
	virtual bool load_font_chars(std::istream& stream) = 0;

	cmn::uint get_char(utf32_char c) const;

	TextureAtlas ta_;
	std::map<utf32_char, cmn::uint> chars_;
	std::string name_;
	cmn::uint height_;
};

}}

#endif
