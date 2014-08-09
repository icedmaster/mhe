#ifndef __UNLIT_MATERIAL_SYSTEM_HPP__
#define __UNLIT_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "math/matrix.hpp"

namespace mhe {

class MHE_EXPORT UnlitMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("unlit");
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context);
	void close() {}

	void update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t *indexes, size_t count);
	void setup(Context& context, Node* materials, ModelContext* model_contexts, size_t count);
	void destroy(Context& context, Node* nodes, size_t count);
private:
	void setup_uniforms(Material& material, Context& context, const ModelContext& model_context);

	size_t layout() const;

	size_t shader() const
	{
		return shader_.shader_program_handle;
	}

	Shader shader_;
	UniformBuffer::IdType transform_uniform_;
};

}

#endif
