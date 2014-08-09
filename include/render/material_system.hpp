#ifndef __MATERIAL_SYSTEM_HPP__
#define __MATERIAL_SYSTEM_HPP__

#include "core/ref_counter.hpp"
#include "core/hash.hpp"

namespace mhe {

struct Context;
struct Material;
struct MaterialInstance;
struct RenderContext;
struct ModelContext;
struct Node;
struct Mesh;
class Transform;

struct MaterialSystemContext
{
	std::string shader_name;
};

class MaterialSystem : public ref_counter
{
public:
	virtual ~MaterialSystem() {}
	
	virtual bool init(Context& context, const MaterialSystemContext& material_system_context) = 0;
	virtual void close() = 0;

	virtual void setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count) = 0;
	virtual void destroy(Context& context, Node* nodes, size_t count) = 0;
	virtual void update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t* transform_indices, size_t count) = 0;

	uint8_t id() const
	{
		return id_;
	}

	void set_id(uint8_t id)
	{
		id_ = id;
	}

	uint8_t priority() const
	{
		return priority_;
	}

	void set_priority(uint8_t priority)
	{
		priority_ = priority;
	}

	hash_type name() const
	{
		return name_impl();
	}
protected:
	void standart_material_setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count, size_t textures_number);
	Transform& transform(const Node& node, Transform* transforms, size_t* indexes) const;
private:
	virtual hash_type name_impl() const = 0;
	virtual void setup_uniforms(Material& /*material*/, Context& /*context*/, const ModelContext& /*model_context*/) {}
	virtual size_t layout() const = 0;
	virtual size_t shader() const = 0;

	uint8_t id_;
	uint8_t priority_;
};

#define SETUP_MATERIAL(mname) public: static hash_type name() {return hash(mname);} private: hash_type name_impl() const {static hash_type n = hash(mname); return n;}

}

#endif
