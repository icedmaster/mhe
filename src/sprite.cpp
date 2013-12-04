#include "sprite.hpp"
#include "utils/sysutils.hpp"
#include "utils/logutils.hpp"
#include "utils/graphics_utils.hpp"

namespace mhe {

void Sprite::on_material_changed()
{
	SpriteBase::update_buffers();
}

float SpriteBase::width() const
{
	if (x_size_) return x_size_;
	return texture_impl()->width();
}

float SpriteBase::height() const
{
	if (y_size_) return y_size_;
	return texture_impl()->height();
}

void SpriteBase::init()
{
	set_mesh_impl(utils::create_sprite_mesh());
}

void SpriteBase::update_buffers()
{
	if (materials_number_impl() == 0) return; // update will be later 

	if (mesh_impl() == nullptr)
		init();

	float x_sz = x_size_, y_sz = y_size_;
	if (!x_size_ || !y_size_)
	{
		x_sz = texture_impl()->width();
		y_sz = texture_impl()->height();
	}

	matrixf initial = matrixf::scaling_matrix(vector3<float>(x_sz, y_sz, 1.0f));
	initial *= matrixf::translation_matrix(0, 0, z_order_);
	set_initial_transform_impl(matrixf::identity());
	set_transform_impl(matrixf::identity());
	set_initial_transform_impl(initial);
	rebuild_impl();
}

}
