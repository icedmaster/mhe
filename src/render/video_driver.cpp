#include "render/video_driver.hpp"

#include "impl/system_factory.hpp"

#include "render/node.hpp"
#include "render/context.hpp"

#include "debug/profiler.hpp"

namespace mhe {

// helper stats class
void Driver::Stats::update(size_t tris)
{
	++batches_;
	tris_ += tris;
}

void Driver::Stats::update_frames()
{
	++frames_;
}

Driver::Driver() :
	impl_(SystemFactory::instance().create_driver())
{
	::memset(&state_, 0xff, sizeof(state_));
}

void Driver::clear(bool clear_color, bool clear_depth, bool clear_stencil,
		const colorf& color, float depth, uint stencil)
{
	if (clear_color)
	{
		impl_->set_clear_color(color);
		impl_->clear_color();
	}

	if (clear_depth)
	{
		impl_->clear_depth();
		NOT_IMPLEMENTED(depth);
	}

	impl_->clear_stencil();
	NOT_IMPLEMENTED(stencil);
}

void Driver::reset()
{
	stats_.reset();
	impl_.reset(SystemFactory::instance().create_driver());
}

void Driver::render(const Context& context, const Node* nodes, size_t count)
{
	ProfilerElement pe("driver.render");
	ASSERT(nodes, "Invalid nodes");
	for (size_t i = 0; i < count; ++i)
	{
		const Node& node = nodes[i];
		uint16_t draw_call_data_id = node.main_pass.draw_call_data;
		uint8_t material_system_id = node.main_pass.material.material_system;
		Material::IdType material_id = node.main_pass.material.id;

		size_t additional_pass_index = 0;
		AdditionalPasses passes;
		if (context.additional_passes_pool.is_valid(node.additional_passes))
			passes = context.additional_passes_pool.get(node.additional_passes);
		while (true)
		{
			if (context.draw_call_data_pool.is_valid(draw_call_data_id))
			{
				const DrawCallData& draw_call_data = context.draw_call_data_pool.get(draw_call_data_id);
				if (draw_call_data.render_target != default_render_target)
				{
					if (state_.render_target != draw_call_data.render_target)
					{
						state_.render_target = draw_call_data.render_target;
						impl_->set_render_target(context.render_target_pool.get(draw_call_data.render_target - 1));
					}
				}
				else
				{
					if (state_.render_target != default_render_target)
					{
						state_.render_target = default_render_target;
						impl_->set_default_render_target();
					}
				}

				if (draw_call_data.command != nullptr)
				{
					if (!draw_call_data.command->execute())
						break;
				}

				if (state_.state != draw_call_data.state)
				{
					impl_->set_state(context.render_state_pool.get(draw_call_data.state));
					state_.state = draw_call_data.state;
				}
			}
			else
			{
				if (state_.render_target != default_render_target)
				{
					impl_->set_default_render_target();
					state_.render_target = default_render_target;
				}
			}

			const Material& material =
				context.materials[material_system_id].get(material_id);

			if (state_.shader_program != material.shader_program)
			{
				impl_->set_shader_program(context.shader_pool.get(material.shader_program));
				state_.shader_program = material.shader_program;
			}
			
			if (state_.vertex_buffer != node.mesh.vbuffer)
			{
				impl_->set_vertex_buffer(context.vertex_buffer_pool.get(node.mesh.vbuffer));
				state_.vertex_buffer = node.mesh.vbuffer;
			}
			
			if (state_.index_buffer != node.mesh.ibuffer)
			{
				impl_->set_index_buffer(context.index_buffer_pool.get(node.mesh.ibuffer));
				state_.index_buffer = node.mesh.ibuffer;
			}
			
			//if (state_.layout != node.mesh.layout)
			{
				impl_->set_layout(context.layout_pool.get(node.mesh.layout));
				state_.layout = node.mesh.layout;
			}
			
			for (size_t j = 0; j < material_textures_number; ++j)
			{
				if (material.textures[j].id == Texture::invalid_id || material.textures[j].id == state_.textures[j])
					continue;
				const Texture& texture = context.texture_pool.get(material.textures[j].id);
				impl_->set_texture(texture, j);
				state_.textures[j] = material.textures[j].id;
			}
			for (size_t j = 0; j < material_uniforms_number; ++j)
			{
				if (material.uniforms[j] == UniformBuffer::invalid_id || material.uniforms[j] == state_.uniforms[j])
					continue;
				const UniformBuffer& uniform = context.uniform_pool.get(material.uniforms[j]);
				impl_->set_uniform(uniform);
				state_.uniforms[j] = material.uniforms[j];
			}
			impl_->draw(node.mesh.render_data);
			stats_.update(node.mesh.render_data.elements_number);

			if (passes.passes.empty() || additional_pass_index >= passes.passes.size())
				break;
			material_id = passes.passes[additional_pass_index].material;
			draw_call_data_id = passes.passes[additional_pass_index].draw_call_data;
			++additional_pass_index;
		}
	}
}

void Driver::begin_render()
{
}

void Driver::end_render()
{
	stats_.update_frames();
	impl_->flush();
}

}
