#ifndef __MATERIAL_SYSTEM_HPP__
#define __MATERIAL_SYSTEM_HPP__

#include "core/ref_counter.hpp"
#include "core/hash.hpp"
#include "core/factory.hpp"

#include "shader_program.hpp"

namespace mhe {

struct Context;
struct Material;
struct MaterialInstance;
struct RenderContext;
struct ModelContext;
struct Node;
struct Mesh;
struct TextureInstance;
class Transform;

static const size_t max_material_definitions = 8;

struct MaterialSystemContext
{
	std::string shader_name;
	std::string defs[max_material_definitions];
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

	virtual void set_texture(const TextureInstance& /*texture*/) {}
	virtual void set_texture(size_t /*unit*/, const TextureInstance& /*texture*/) {}

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
	void additional_passes_setup(Context& context, Node* nodes, size_t count);
	Transform& transform(const Node& node, Transform* transforms, size_t* indexes) const;

	void set_layout(size_t layout)
	{
		layout_ = layout;
	}

	size_t layout() const
	{
		return layout_;
	}

	void set_shader(Shader shader)
	{
		shader_ = shader;
	}

	Shader& shader()
	{
		return shader_;
	}

	UberShader& ubershader(const Context& context) const;
	ShaderProgram& default_program(const Context& context) const;
private:
	virtual hash_type name_impl() const = 0;
	virtual void setup_uniforms(Material& /*material*/, Context& /*context*/, const ModelContext& /*model_context*/) {}

	Shader shader_;
	size_t layout_;
	uint8_t id_;
	uint8_t priority_;
};

typedef Factory<MaterialSystem> MaterialSystemFactory;

#define SETUP_MATERIAL(mname) public: static hash_type name() {return hash(mname);} private: hash_type name_impl() const {static hash_type n = hash(mname); return n;}

}

#endif