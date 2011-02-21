#include "texture.hpp"

using namespace mhe;

void Texture::rebuild_texture()
{
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);
	//switch (ft_)
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int format = (im_->bpp() == 32) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, im_->bpp() >> 3, im_->width(), im_->height(),
				 0, format, GL_UNSIGNED_BYTE, &(im_->get()[0]));

    w_ = im_->width();
    h_ = im_->height();
	if (st_ == NotStore)
		im_.reset();
    binded_ = true;
}
