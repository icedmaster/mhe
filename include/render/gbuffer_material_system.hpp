#ifndef __GBUFFER_MATERIAL_SYSTEM_HPP__
#define __GBUFFER_MATERIAL_SYSTEM_HPP__

#include "material_system.hpp"
#include "shader_program.hpp"
#include "render_buffer.hpp"
#include "render_target.hpp"
#include "node.hpp"

namespace mhe {

class MHE_EXPORT GBufferFillMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("gbuffer_fill");
public:
	GBufferFillMaterialSystem();
	bool init(Context& context, const MaterialSystemContext& material_system_context);
	void close();

	void setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count);
	void destroy(Context& context, Node* nodes, size_t count);
	void update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t* transform_indices, size_t count);

	RenderTarget::IdType render_target() const
	{
		return render_target_;
	}
private:
	class MHE_EXPORT ClearCommand : public RenderCommand
	{
	public:
		void set_driver(Driver* driver)
		{
			driver_ = driver;
		}

		bool execute();
	private:
		Driver* driver_;
	};

	void setup_uniforms(Material& material, Context& context, const ModelContext& model_context);

	size_t layout() const;

	size_t shader() const
	{
		return shader_.shader_program_handle;
	}

	ClearCommand clear_command_;
	Shader shader_;
	UniformBuffer::IdType transform_uniform_;
	RenderTargetDesc gbuffer_desc_;
	RenderTarget::IdType render_target_;
};

class MHE_EXPORT GBufferDrawMaterialSystem : public MaterialSystem
{
	SETUP_MATERIAL("gbuffer_draw");
public:
	GBufferDrawMaterialSystem(RenderTarget::IdType render_target) :
		render_target_(render_target)
	{}

	bool init(Context& context, const MaterialSystemContext& material_system_context);
	void close();

	void setup(Context& context, Node* nodes, ModelContext* model_contexts, size_t count);
	void destroy(Context& context, Node* nodes, size_t count);
	void update(Context& context, RenderContext& render_context, Node* nodes, Transform* transforms, size_t* transform_indices, size_t count);
private:
	size_t layout() const;

	size_t shader() const
	{
		return shader_.shader_program_handle;
	}

	RenderTarget::IdType render_target_;
	Shader shader_;
};

}

#endif
