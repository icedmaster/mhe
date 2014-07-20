#ifndef __MATERIAL_SYSTEM_HPP__
#define __MATERIAL_SYSTEM_HPP__

#include "core/ref_counter.hpp"

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
	virtual void update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t count) = 0;

	uint8_t id() const
	{
		return id_;
	}

	void set_id(uint8_t id)
	{
		id_ = id;
	}
private:
	uint8_t id_;
};

}

#endif
