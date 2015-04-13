#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "window_system.hpp"
#include "video_driver.hpp"
#include "render_buffer.hpp"
#include "render_state.hpp"
#include "render_target.hpp"
#include "material_system.hpp"
#include "material.hpp"
#include "node.hpp"
#include "core/pool.hpp"
#include "core/types_cast.hpp"
#include "core/config.hpp"
#include "res/resource_managers.hpp"

namespace mhe {

typedef Pool<VertexBuffer, 4096, uint16_t> VertexBufferPool;
typedef Pool<IndexBuffer, 4096, uint16_t> IndexBufferPool;
typedef Pool<UniformBuffer, 4096, uint16_t> UniformPool;
typedef Pool<Layout, 128, uint16_t> LayoutPool;
typedef Pool<ShaderProgram, max_shader_programs_number, ShaderProgram::IdType> ShaderPool;
typedef Pool<UberShader, max_ubershaders_number, UberShader::IdType> UbershaderPool;
typedef Pool<RenderState, 4096, uint16_t> RenderStatePool;
typedef Pool<Texture, 4096, uint16_t> TexturePool;
typedef Pool<RenderTarget, max_render_targets_number, RenderTarget::IdType> RenderTargetPool;
typedef Pool< DrawCallData, 4096, uint16_t, StructTypePolicy<DrawCallData, uint16_t> > DrawCallDataPool;

class MaterialSystems
{
private:
	typedef fixed_size_vector< ref_ptr<MaterialSystem>, max_material_systems_number > vector_type;
public:
	typedef vector_type Values;

	void add(MaterialSystem* material_system, uint8_t priority = 128)
	{
		ASSERT(material_system != nullptr, "Invalid material_system");
		material_system->set_id(systems_.size());
		material_system->set_priority(priority);
		systems_.push_back(ref_ptr<MaterialSystem>(material_system));
	}

	template <class T>
	T* get() const
	{
		return get_impl<T>(T::name());
	}

	template <class T>
	T* get(const char* name) const
	{
		return get_impl<T>(hash(name));
	}

	MaterialSystem* get(hash_type name) const
	{
		return get_impl<MaterialSystem>(name);
	}

	MaterialSystem* get(const char* name) const
	{
		return get_impl<MaterialSystem>(hash(name));
	}

    MaterialSystem* get(const string& name) const
    {
        return get(name.c_str());
    }

	MaterialSystem* get(uint8_t id) const
	{
		ASSERT(id < systems_.size(), "Invalid material_id " << id);
		return systems_[id].get();
	}

	vector_type& get_all_materials()
	{
		return systems_;
	}

    void disable_all()
    {
        for (size_t i = 0; i < systems_.size(); ++i)
            systems_[i]->disable();
    }
private:
	template <class T>
	T* get_impl(hash_type name) const
	{
		for (size_t i = 0; i < systems_.size(); ++i)
		{
			if (systems_[i]->name() == name)
				return checked_static_cast<T*>(systems_[i].get());
		}
		return nullptr;
	}

	vector_type systems_;
};

typedef Pool< Material, 1024, uint16_t, StructTypePolicy<Material, uint16_t> > MaterialPool;

struct Context
{
	WindowSystem window_system;
	Driver driver;

	MeshManager mesh_manager;
	ShaderManager shader_manager;
	TextureManager texture_manager;

	VertexBufferPool vertex_buffer_pool;
	IndexBufferPool index_buffer_pool;
	UniformPool uniform_pool;
	LayoutPool layout_pool;
	ShaderPool shader_pool;
	UbershaderPool ubershader_pool;
	RenderStatePool render_state_pool;
	RenderTargetPool render_target_pool;
	TexturePool texture_pool;
	DrawCallDataPool draw_call_data_pool;

	MaterialSystems material_systems;
	MaterialPool materials[max_material_systems_number];
	MaterialManager material_manager;
};

}

#endif
