#include "gui/bmfont.hpp"

#include <fstream>
#include "utils/options_parser.hpp"
#include "utils/logutils.hpp"
#include "texture_manager.hpp"

namespace mhe {
namespace gui {

	bool BMFont::load(const std::string& filename)
	{
		std::ifstream f(filename.c_str());
		if (!f.is_open())
			return false;

		std::string str;
		// first string - info
		std::getline(f, str);
		mhe::utils::OptionsParser op(str, ' ');
		op.get_string("face", name_, "");
		name_ = utils::replace(name_, '"');
		op.get<cmn::uint>("size", height_, 0);
		// second string - draw info
		std::getline(f, str);
		// third string - texture info
		std::getline(f, str);
		op.process(str, ' ');
		std::string texture_filename;
		op.get_string("file", texture_filename, "");
		if (texture_filename.empty())
			return false;
        texture_filename = utils::replace(texture_filename, '"');
		load_texture(texture_filename);
		// forth string - number of chars
		std::getline(f, str);
	    op.process(str, ' ');
		int chars_num;
		op.get_integer("count", chars_num, 0);

		std::vector<float> texc;
		texc.reserve(chars_num * 8);

		for (int i = 0; i < chars_num; ++i)
		{
			std::getline(f, str);
			load_char(str, texc);
		}
		f.close();

		ta_.set_indicies(&texc[0], texc.size());
		return true;
	}

	void BMFont::load_texture(const std::string& filename)
	{
		TextureManager tm;
		ta_.set_texture(tm.get(filename));
	}

	void BMFont::load_char(const std::string& str,
						   std::vector<float>& texc)
	{
		mhe::utils::OptionsParser op(str, ' ');
		unsigned int id, width, height, x, y;
		op.get<unsigned int>("id", id, 0);
		op.get<unsigned int>("x", x, 0);
		op.get<unsigned int>("y", y, 0);
		op.get<unsigned int>("width", width, 0);
		op.get<unsigned int>("height", height, 0);

		const float texture_width  = ta_.texture()->width();
		const float texture_height = ta_.texture()->height();
		y = texture_height - y;

		float tc[8];
		tc[0] = static_cast<float>(x) / texture_width;
		tc[1] = static_cast<float>(y - height) / texture_height;
		tc[2] = tc[0];
		tc[3] = static_cast<float>(y) / texture_height;
		tc[4] = static_cast<float>(x + width) / texture_width;
		tc[5] = tc[3];
		tc[6] = tc[4];
		tc[7] = tc[1];
		texc.insert(texc.end(), tc, tc + 8);

		size_t sz = chars_.size();
		chars_[id] = sz;
	}

	cmn::uint BMFont::get_char(cmn::uint sym) const
	{
		std::map<cmn::uint, cmn::uint>::const_iterator it = chars_.find(sym);
		if (it == chars_.end())
		{
			assert(0);
			return 0;
		}
		return it->second;
	}

	// must called from 2d mode
	void BMFont::print(const boost::shared_ptr<Driver>& driver, const std::wstring& text,
					   const vector2<int>& coord, const colorf& color)
	{
		std::vector<float> t;	// texture coordinates
		std::vector<cmn::uint> ibuf;
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
			cmn::uint sym = (cmn::uint)text[i];
			cmn::uint ind = get_char(sym);
			const v2d& sz = ta_.get_size(ind);
			const Texture::texcoord& tc = ta_.get(ind);
			t.insert(t.end(), tc.data(), tc.data() + tc.size());
			// prepare vertex coordinates
			boost::array<float, 12> cc;
			cc[0] = coord.x() + x_offs;		cc[1] = coord.y();
			cc[3] = cc[0];					cc[4] = coord.y() + sz.y();
			cc[6] = cc[0] + sz.x(); 		cc[7] = cc[4];
			cc[9] = cc[6];					cc[10] = cc[1];
			cc[2] = cc[5] = cc[8] = cc[11] = 0.0;
			v.insert(v.end(), cc.data(), cc.data() + cc.size());
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
        driver->enable_blending(ALPHA_ONE_MINUS_ALPHA);

		ta_.texture()->prepare();
		driver->draw(&v[0], 0, &t[0], &c[0], &ibuf[0], ibuf.size());
		ta_.texture()->clean();

		driver->unmask_zbuffer();
        driver->disable_blending();
	}
}
}
