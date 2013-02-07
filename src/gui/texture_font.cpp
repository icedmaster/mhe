#include "gui/texture_font.hpp"

#include <fstream>
#include "utils/global_log.hpp"
#include "texture_manager.hpp"

namespace mhe {
namespace gui {

bool TextureFont::load(const std::string& filename)
{
	std::ifstream f(filename.c_str());
	if (!f.is_open())
	{
		WARN_LOG("Can't open file:" << filename);
		return false;
	}
	std::string texture_filename;
	bool loaded = load_font_parameters(f, texture_filename);
	if (!loaded)
	{
		WARN_LOG("Can't load font parameters from file:" << filename);
	}
	else
	{
		load_texture(texture_filename);
		loaded = load_font_chars(f);
		if (!loaded)
			WARN_LOG("Can't load font chars from file:" << filename);
	}
	f.close();
	return loaded;
}

void TextureFont::print(const boost::shared_ptr<Driver>& driver, const utf8_string& text,
						const vector2<float>& position, const colorf& color)
{
	std::vector<float> t;	// texture coordinates
	std::vector<cmn::uint> ibuf;  // indicies
	std::vector<float> v;	// vertexes
	const size_t len = text.length();
	v.reserve(len * 12);
	t.reserve(len * 8);
	ibuf.reserve(len * 6);
	std::vector<float> c;
	c.reserve(len * 16);

	float x_offs = 0;
	for (size_t i = 0; i < len; ++i)
	{
		utf8_char sym = text[i];
		cmn::uint ind = get_char(sym);
		const v2d& sz = ta_.get_size(ind);
		const std::vector<float>& tc = ta_.get(ind);
		t.insert(t.end(), tc.data(), tc.data() + tc.size());
		const vector2<float>& coord = position;
		// prepare vertex coordinates	
		float cc[12];
		cc[0] = coord.x() + x_offs;		cc[1] = coord.y();
		cc[3] = cc[0];					cc[4] = coord.y() + sz.y();
		cc[6] = cc[0] + sz.x(); 		cc[7] = cc[4];
		cc[9] = cc[6];					cc[10] = cc[1];
		cc[2] = cc[5] = cc[8] = cc[11] = 0.0;
		v.insert(v.end(), cc, cc + 12);
		x_offs += (sz.x() + 1);	// TODO: need to check outline

		// colors are the same for all vertexes
		for (int j = 0; j < 4; ++j)
			c.insert(c.end(), color.get(), color.get() + 4);
		// setup indicies
		cmn::uint isz = i * 4;
		cmn::uint indicies[6] = {isz, isz + 1, isz + 2,
								 isz + 2, isz + 3, isz};
		ibuf.insert(ibuf.end(), indicies, indicies + 6);
	}

	driver->mask_zbuffer();
	driver->enable_blending(alpha_one_minus_alpha);

	ta_.texture()->prepare();
	driver->draw(&v[0], 0, &t[0], &c[0], &ibuf[0], ibuf.size());
	ta_.texture()->clean();

	driver->unmask_zbuffer();
	driver->disable_blending();
}

void TextureFont::load_texture(const std::string& filename)
{
	TextureManager temp_manager;
	ta_.set_texture(temp_manager.get(filename));
}

void TextureFont::add_char_for_index(utf8_char c, const rect<float>& rect)
{
	const float texture_width  = ta_.texture()->width();
	const float texture_height = ta_.texture()->height();

	const float x = rect.ll().x();
	const float y = rect.ll().y();
	const float height = rect.height();
	const float width = rect.width();

	float tc[8];
	tc[0] = x / texture_width;
	tc[1] = y - height / texture_height;
	tc[2] = tc[0];
	tc[3] = y / texture_height;
	tc[4] = x + width / texture_width;
	tc[5] = tc[3];
	tc[6] = tc[4];
	tc[7] = tc[1];

	ta_.add_indicies(tc, 8);
	chars_[c] = chars_.size();
}

cmn::uint TextureFont::get_char(utf8_char c) const
{
	std::map<utf8_char, cmn::uint>::const_iterator it = chars_.find(c);
	assert(it != chars_.end());
	return it->second;
}

}}
