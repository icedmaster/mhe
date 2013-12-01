#ifndef __RENDERABLE_HPP__
#define __RENDERABLE_HPP__

#include <vector>
#include "types.hpp"
#include "material.hpp"
#include "transform.hpp"
#include "mesh.hpp"
#include "engine_config.hpp"

namespace mhe {

class Driver;

class Renderable : public Transform
{
	friend class Driver;	
public:
	enum
	{
		default_render = 0, 
		batching_disabled = (1 << 3)
	};

	typedef fixed_size_vector<float, initial_number_of_vertexes> vertexes_container;
	typedef fixed_size_vector<float, initial_number_of_texcoords> texcoord_container;
public:
	static void batch(Renderable& to, const Renderable& from);
public:
	Renderable() :
		color_(color_white), render_flags_(0)
	{ 
	}

	void set_material(const material_ptr& material);
	void add_material(const material_ptr& material);

	material_ptr material() const
	{
		return materials_.front();
	}

	size_t materials_number() const
	{
		return materials_.size();
	}

	size_t textures_number() const
	{
		return texcoord_.size();
	}

	material_ptr material_at(size_t index) const
	{
		return materials_[index];
	}

	const material_ptr* materials() const
	{
		return materials_.data();
	}

	const boost::shared_ptr<Texture> texture() const
	{
		return materials_.front()->texture();
	}

	void set_mesh(const mesh_ptr& mesh);

	mesh_ptr mesh() const
	{
		return mesh_;
	}

	void set_color(const colorf& color);

	const colorf& color() const
	{
		return color_;
	}

	texcoord_container texcoord() const
	{
		return texcoord_[0];
	}

	texcoord_container texcoord_at(size_t index) const
	{
		return texcoord_[index];
	}

	void set_texcoord(const texcoord_container& coord)
	{
		texcoord_[0] = coord;
	}

	vertexes_container colorcoord() const
	{
		return colorcoord_;
	}

	void clear_render_flags()
	{
		render_flags_ = default_render;
	}

	void set_batching_disabled()
	{
		render_flags_ |= batching_disabled;
	}

	void set_batching_enabled()
	{
		render_flags_ &= ~batching_disabled;
	}

	bool is_batching_disabled() const
	{
		return (render_flags_ & batching_disabled);
	}

	uint32_t render_flags() const
	{
		return render_flags_;
	}

	void clear();

	bool is_material_equals(const Renderable& other) const;
protected:
	texcoord_container& rtexcoord()
	{
		assert(!texcoord_.empty());
		return texcoord_[0];
	}

	texcoord_container& rtexcoord_at(size_t index)
	{
		assert(texcoord_.size() > index);
		return texcoord_[index];
	}

	material_ptr material_by_texture(size_t texture_index) const;

	void update_color_buffer();
	void update_buffers(bool update_color = true);
private:
	virtual void on_material_changed() {}

	void resize_texcoord();
	
	fixed_size_vector<texcoord_container, max_materials_number * initial_number_of_textures> texcoord_;
	vertexes_container colorcoord_;
	fixed_size_vector<material_ptr, max_materials_number> materials_;
	mesh_ptr mesh_;
	colorf color_;
	uint32_t render_flags_;
};

inline Renderable::vertexes_container vector_to_vertex_container(const std::vector<float>& v)
{
	Renderable::vertexes_container result(v.begin(), v.end());
	return result;
}

inline Renderable::texcoord_container vector_to_texcoord_container(const std::vector<float>& v)
{
	Renderable::texcoord_container result(v.begin(), v.end());
	return result;
}

}

#endif
