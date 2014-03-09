#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include <vector>
#include <map>
#include "node.hpp"

namespace mhe {

class SpriteBase
{
public:
	SpriteBase() : node_impl_(nullptr), 
				   x_size_(0.0), y_size_(0.0),
				   z_order_(0)
	{}

	virtual ~SpriteBase() {}

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
protected:
	void update_buffers();
private:
	virtual void set_mesh_impl(const mesh_ptr& mesh) = 0;
	virtual texture_ptr texture_impl() const = 0;
	virtual void set_transform_impl(const matrixf& m) = 0;
	virtual void rebuild_impl() = 0;
	virtual size_t materials_number_impl() const = 0;
	virtual mesh_ptr mesh_impl() const = 0;
private:
	void init();

	Node* node_impl_;
	float x_size_, y_size_;
	float z_order_;
};

class Sprite : public SpriteBase, public Node
{
private:
	void set_mesh_impl(const mesh_ptr& mesh)
	{
		Node::set_mesh(mesh);
	}

	texture_ptr texture_impl() const
	{
		return Node::texture();
	}

	void set_transform_impl(const matrixf& m)
	{
		Node::set_transform(m);
	}

	void rebuild_impl()
	{
		Node::rebuild();
	}

	size_t materials_number_impl() const
	{
		return Node::materials_number();
	}

	mesh_ptr mesh_impl() const
	{
		return Node::mesh();
	}
private:
	void on_material_changed();
};
}

#endif
