#include "sprite.hpp"
#include "utils/sysutils.hpp"
#include "utils/logutils.hpp"
#include "utils/graphics_utils.hpp"

namespace mhe
{

Node* Sprite::clone_impl() const
{
	Sprite* cloned = new Sprite(*this);
	return cloned;
}

void Sprite::on_material_changed()
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
	Renderable::set_mesh(utils::create_sprite_mesh());
}

void Sprite::update_buffers()
{
	if (materials_number() == 0) return; // update will be later 

	float x_sz = x_size_, y_sz = y_size_;
	if (!x_size_ || !y_size_)
	{
		x_sz = texture()->width();
		y_sz = texture()->height();
	}

	matrixf initial = matrixf::scaling_matrix(vector3<float>(x_sz, y_sz, 1.0f));
	initial *= matrixf::translation_matrix(0, 0, z_order_);
	Renderable::set_initial(matrixf::identity());
	Renderable::set_transform(matrixf::identity());
	Renderable::set_initial(initial);
	Renderable::update_buffers();
}

}
