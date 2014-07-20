#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "window_system.hpp"
#include "video_driver.hpp"
#include "render_buffer.hpp"
#include "render_state.hpp"
#include "material_system.hpp"
#include "material.hpp"
#include "core/pool.hpp"
#include "core/types_cast.hpp"
#include "core/config.hpp"
#include "res/resource_managers.hpp"

namespace mhe {

typedef Pool<VertexBuffer, 4096, uint16_t> VertexBufferPool;
typedef Pool<IndexBuffer, 4096, uint16_t> IndexBufferPool;
typedef Pool<UniformBuffer, 4096, uint16_t> UniformPool;
typedef Pool<Layout, 128, uint16_t> LayoutPool;
typedef Pool<ShaderProgram, 128, uint16_t> ShaderPool;
typedef Pool<RenderState, 4096, uint16_t> RenderStatePool;

template <class K, class V, class I = uint8_t>
class MapWrapper
{
	typedef ref_ptr<V> value_type;
	typedef std::map<K, value_type> map_type;
	typedef fixed_size_vector<V*, sizeof(I) * 256> vector_type;
public:
	typedef vector_type Values;

	MapWrapper() :
		next_id_(0)
	{}

	I add(K key, V* value)
	{
		map_[key] = value_type(value);
		cache_.push_back(value);
		value->set_id(next_id_);
		return next_id_++;
	}

	template <class T>
	I add(T* value)
	{
		return add(T::name(), value);
	}

	V* get(K key) const
	{
		typename map_type::const_iterator it = map_.find(key);
		return it == map_.end() ? nullptr : it->second.get();
	}

	template <class T>
	T* get() const
	{
		V* v = get(T::name());
		return checked_static_cast<T*>(v);
	}

	vector_type& get_all_materials()
	{
		return cache_;
	}
private:
	map_type map_;
	vector_type cache_;
	I next_id_;
};

typedef MapWrapper<const char*, MaterialSystem> MaterialSystems;
typedef Pool< Material, 1024, uint16_t, StructTypePolicy<Material, uint16_t> > MaterialPool;

struct Context
{
	WindowSystem window_system;
	Driver driver;

	MeshManager mesh_manager;
	ShaderManager shader_manager;

	VertexBufferPool vertex_buffer_pool;
	IndexBufferPool index_buffer_pool;
	UniformPool uniform_pool;
	LayoutPool layout_pool;
	ShaderPool shader_pool;
	RenderStatePool render_state_pool;

	MaterialSystems material_systems;
	MaterialPool materials[max_material_systems_number];
};

}

#endif
