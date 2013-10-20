#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include <vector>
#include <map>
#include "node.hpp"

namespace mhe
{

class Sprite : public Node
{
public:
	Sprite() : Node(16, 6, 8), x_size_(0.0), y_size_(0.0),
			   z_order_(0)
	{
		init();
	}

	Sprite(const Sprite& sprite);

	void set_size(float size)
	{
		x_size_ = y_size_ = size;
		update_buffers();
	}

	void set_size(float xsize, float ysize)
	{
		x_size_ = xsize;
		y_size_ = ysize;
        update_buffers();
	}

	float width() const;
	float height() const;			

	void set_z_order(int order)
	{
		z_order_ = order;
	}

	int z_order() const
	{
		return z_order_;
	}
private:
	void on_material_changed();

	Node* clone_impl() const;
private:
	void init();
	void update_buffers();

	float x_size_, y_size_;
	int z_order_;
};
}

#endif
