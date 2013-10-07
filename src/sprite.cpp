#include "sprite.hpp"
#include "utils/sysutils.hpp"
#include "utils/logutils.hpp"

namespace mhe
{

Sprite::Sprite(const Sprite& sprite) :
	x_size_(sprite.x_size_), y_size_(sprite.y_size_),
	z_order_(sprite.z_order_)
{
	set_texture(sprite.texture());
	init();
}

Node* Sprite::clone_impl() const
{
	Sprite* cloned = new Sprite(*this);
	return cloned;
}

void Sprite::on_texture_changed()
{
	update_buffers();
}

float Sprite::width() const
{
	if (x_size_) return x_size_;
	return texture()->width();
}

float Sprite::height() const
{
	if (y_size_) return y_size_;
	return texture()->height();
}

void Sprite::init()
{
	Node::rvertexcoord().resize(12);
	Renderable::vertex_container& n = Node::rnormalscoord();
	n.resize(12);
	n[0] = 0.0; n[1] = 0.0; n[2] = 1.0;
	n[3] = 0.0; n[4] = 0.0; n[5] = 1.0;
	n[6] = 0.0; n[7] = 0.0; n[8] = 1.0;
	n[9] = 0.0; n[10] = 0.0; n[11] = 1.0;
	
	Renderable::indexes_container& i = Node::rindicies();
	i.resize(6);
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 2; i[4] = 3; i[5] = 0;

	Renderable::texcoord_container& t = Node::rtexcoord();
	t.resize(8);
	t[0] = 0.0; t[1] = 0.0; t[2] = 0.0; t[3] = 1.0;
	t[4] = 1.0; t[5] = 1.0; t[6] = 1.0; t[7] = 0.0;

	set_mask_z_buffer();
	set_blending_enabled();
	set_blend_mode(alpha_one_minus_alpha);
}

void Sprite::update_buffers()
{
	Renderable::vertex_container& v = Node::rvertexcoord();
	float x_sz = x_size_, y_sz = y_size_;
	if (!x_size_ || !y_size_)
	{
		x_sz = texture()->width();
		y_sz = texture()->height();
	}

	v[0] = 0.0; v[1] = 0.0; v[2] = z_order_;
	v[3] = 0.0; v[4] = y_sz; v[5] = z_order_;
	v[6] = x_sz; v[7] = y_sz; v[8] = z_order_;
	v[9] = x_sz; v[10] = 0.0; v[11] = z_order_;
	update_color_buffer();
}

}
