#include "renderable.hpp"

namespace mhe {

void Renderable::set_material(const material_ptr& material)
{
	materials_.resize(1);
	texcoord_.resize(material->textures_number());
	materials_[0] = material;
	if (mesh_ != nullptr)
	{
		for (size_t i = 0; i < material->textures_number(); ++i)
			texcoord_[i].resize(mesh_->uv.size());
	}
	on_material_changed();
}

void Renderable::add_material(const material_ptr& material)
{
	materials_.push_back(material);
	size_t current_textures_number = texcoord_.size();
	for (size_t i = 0; i < material->textures_number(); ++i)
		texcoord_.push_back(texcoord_container());
	if (material->shader() != materials_.front()->shader())
		set_batching_disabled();
	if (mesh_ != nullptr)
	{
		for (size_t i = 0; i < material->textures_number(); ++i)
			texcoord_[current_textures_number + i].resize(mesh_->uv.size());
	}
	on_material_changed();
}

void Renderable::set_mesh(const mesh_ptr& mesh)
{
	mesh_ = mesh;
	if (materials_.empty()) return;
	resize_texcoord();
	update_buffers();
}

void Renderable::set_color(const colorf& color)
{
	color_ = color;
	update_color_buffer();
}

void Renderable::clear()
{
	render_flags_ = default_render;
	materials_.clear();
	texcoord_.clear();
	colorcoord_.clear();
	mesh_->clear();
}

bool Renderable::is_material_equals(const Renderable& other) const
{
	if (materials_.size() != other.materials_number()) return false;
	for (size_t i = 0; i < materials_.size(); ++i)
	{
		if ( *(materials_[i]) != *(other.material_at(i)) ) return false;
	}
	return true;
}

material_ptr Renderable::material_by_texture(size_t texture_index) const
{
	size_t index = 0;
	for (size_t i = 0; i < materials_.size(); ++i)
	{
		size_t next = index + materials_[i]->textures_number();
		if ( (texture_index >= index) && (texture_index < next) )
			return materials_[i];
		index = next;
	}
	return material_ptr();
}

void Renderable::update_color_buffer()
{
	if (mesh_ == nullptr) return;
	int count = mesh_->vertexes.size() / 3;
	colorcoord_.resize(count * 4);
	for (int i = 0; i < count * 4; i += 4)
	{
		colorcoord_[i] = color_.r();
		colorcoord_[i + 1] = color_.g();
		colorcoord_[i + 2] = color_.b();
		colorcoord_[i + 3] = color_.a();
	}
}

void Renderable::update_buffers(bool update_color)
{
	if (mesh_ == nullptr)
		return;
	if (update_color) update_color_buffer();
	size_t texture_index = 0;
	const Mesh::texcoord_vector& mesh_uv = mesh_->uv;
	for (size_t i = 0; i < materials_.size(); ++i)
	{			
		for (size_t j = 0; j < materials_[i]->textures_number(); ++j, ++texture_index)
		{
			const std::vector<float>& uv = materials_[i]->uv_at(j);
			float u = uv[0], v = uv[1];
			float w = uv[4], h = uv[5];
			float uk = 1.0 / (w - u);
			float vk = 1.0 / (h - v);
			for (size_t k = 0; k < mesh_uv.size(); k += 2)
			{
				texcoord_[texture_index][k] = mesh_uv[k] * uk; texcoord_[texture_index][k] += u; 
				texcoord_[texture_index][k + 1] = mesh_uv[k + 1] * vk; texcoord_[texture_index][k + 1] += v;			
			}
		}
	}
}

void Renderable::resize_texcoord()
{
	if ( (mesh_ == nullptr) || texcoord_.empty() )
		return;
	for (size_t i = 0; i < texcoord_.size(); ++i)
		texcoord_[i].resize(mesh_->uv.size());
}

// functions
void Renderable::batch(Renderable& to, const Renderable& from)
{
	if (!to.is_material_equals(from))
		return;
	mhe::batch(*(to.mesh_), *(from.mesh_), from.transform());
	for (size_t i = 0; i < to.materials_.size(); ++i)
		to.texcoord_[i].resize(to.mesh_->uv.size());
	to.update_buffers(false);
	to.colorcoord_.insert(to.colorcoord_.end(),
						  from.colorcoord_.begin(), from.colorcoord_.end());
	to.render_flags_ |= from.render_flags_;
}

}
