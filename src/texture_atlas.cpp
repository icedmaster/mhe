#include "texture_atlas.hpp"

namespace mhe
{
	void TextureAtlas::setTexture(const boost::shared_ptr<iTexture>& tex, int sz)
	{
		texture_ = tex;
		if (sz)
			calc_indicies(sz);
	}

	void TextureAtlas::setIndicies(const float* ind, int sz)
	{
		indicies_.clear();
		indicies_.assign(ind, ind + sz);
	}

	texcoord TextureAtlas::get(cmn::uint quad) const
	{
		texcoord a;

		if ( (quad * 8) >= indicies_.size() )
		{
			// without loops
			a[0] = 0.0; a[1] = 0.0;
			a[2] = 0.0; a[3] = 1.0;
			a[4] = 1.0; a[5] = 1.0;
			a[6] = 1.0; a[7] = 0.0;
		}
		else
		{
			size_t q = quad << 3;
			a[0] = indicies_[q];     a[1] = indicies_[q + 1];
			a[2] = indicies_[q + 2]; a[3] = indicies_[q + 3];
			a[4] = indicies_[q + 4]; a[5] = indicies_[q + 5];
			a[6] = indicies_[q + 6]; a[7] = indicies_[q + 7];
		}
		return a;
	}

	v2d TextureAtlas::get_size(cmn::uint quad) const
	{
		const texcoord& tc = get(quad);
		v2d v( (tc[4] - tc[0]) * texture_->width(),
			   (tc[3] - tc[1]) * texture_->height()
			 );
		return v;	
	}

	void TextureAtlas::calc_indicies(int sz)
	{
		const cmn::uint dim = texture_->width();
		const int n = dim / sz;
		const float step = 1.0 / static_cast<float>(n);

		indicies_.clear();
		indicies_.reserve(n * 2);

		for (float y = 0; y < 1.0; y += step)
		{
			for (float x = 0; x < 1.0; x += step)
			{
				indicies_.push_back(x); 		  indicies_.push_back(y);
				indicies_.push_back(x); 		  indicies_.push_back(y + step);
				indicies_.push_back(x + step); indicies_.push_back(y + step);
				indicies_.push_back(x + step); indicies_.push_back(y);
			}
		}
	}
}
