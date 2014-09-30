#ifndef __SKYBOX_MATERIAL_SYSTEM_HPP__
#define __SKYBOX_MATERIAL_SYSTEM_HPP__

#include "core/compiler.hpp"
#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"

namespace mhe {

class MHE_EXPORT SkyboxMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("skybox");
public:
	bool init(Context& context, const MaterialSystemContext& material_system_context);
	void close();

	void setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count);
	void destroy(Context& context, Node* nodes, size_t count);
	void update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t* indexes, size_t count);
private:
	UniformBuffer::IdType transform_uniform_;
};

}

#endif
