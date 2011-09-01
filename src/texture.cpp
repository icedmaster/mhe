#include "texture.hpp"

using namespace mhe;

Texture::Texture() :
	binded_(false)
{
	coord_[0] = 0.0; coord_[1] = 0.0;
	coord_[2] = 0.0; coord_[3] = 1.0;
	coord_[4] = 1.0; coord_[5] = 1.0;
	coord_[6] = 1.0; coord_[7] = 0.0;
}

Texture::~Texture()
{
	if (binded_)
		glDeleteTextures(1, &id_);
}

void Texture::rebuild_texture(const boost::shared_ptr<Image>& im, FilterType/* ft*/)
{
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);
	//switch (ft_)
	//glBindTexture(GL_TEXTURE_2D, id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	w_ = im->width();
    h_ = im->height();

	bool pot_support = is_extension_availible("GL_ARB_texture_non_power_of_two");
	bool rebuild = false;
	if (!pot_support)
	{
		if (!is_pot(w_))
		{
			w_ = nearest_pot(w_);
			rebuild = true;
		}
		if (!is_pot(h_))
		{
			h_ = nearest_pot(h_);
			rebuild = true;
		}
	}

	int format = (im->bpp() == 32) ? GL_RGBA : GL_RGB;

	const int bts = im->bpp() >> 3;
	const char* data = &(im->get()[0]);
    std::vector<char> cdata(0);
	if (rebuild)
	{
		cdata.resize(w_ * h_ * bts, 0);

		cmn::uint wbts = w_ * bts;	// width in bytes
		cmn::uint src_wbts = im->width() * bts;
		for (cmn::uint i = 0; i < im->height(); ++i)
			memcpy(&cdata[i * wbts], data + i * src_wbts, src_wbts);
		data = &cdata[0];
	}

	glTexImage2D(GL_TEXTURE_2D, 0, bts, w_, h_,
				 0, format, GL_UNSIGNED_BYTE, data);
}

void Texture::prepare()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::clean()
{
	glDisable(GL_TEXTURE_2D);
}

boost::shared_ptr<iTexture> Texture::clone() const
{
	boost::shared_ptr<Texture> texture_clone(new Texture);
	texture_clone->w_ = w_;
	texture_clone->h_ = h_;
	texture_clone->id_ = id_;
	// FIXME: we shall not copy bind_ parameter, 'cause we can have a problem with
	// texture deleting
	return texture_clone;
}

