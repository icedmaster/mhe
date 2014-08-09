#include "render/video_driver.hpp"

#include "impl/system_factory.hpp"

#include "render/node.hpp"
#include "render/context.hpp"

namespace mhe {

// helper stats class
void Driver::Stats::update()
{
	++batches_;
}

void Driver::Stats::update_frames()
{
	++frames_;
}

Driver::Driver() :
	impl_(SystemFactory::instance().create_driver())
{
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

	NOT_IMPLEMENTED(clear_stencil);
	NOT_IMPLEMENTED(stencil);
}

void Driver::reset()
{
	stats_.reset();
	impl_.reset(SystemFactory::instance().create_driver());
}

void Driver::render(const Context& context, const Node* nodes, size_t count)
{
	ASSERT(nodes, "Invalid nodes");
	for (size_t i = 0; i < count; ++i)
	{
		const Node& node = nodes[i];
		const Material& material =
			context.materials[node.material.material_system].get(node.material.id);
		
		if (context.draw_call_data_pool.is_valid(node.draw_call_data))
		{
			const DrawCallData& draw_call_data = context.draw_call_data_pool.get(node.draw_call_data);
			if (draw_call_data.render_target != default_render_target)
				impl_->set_render_target(context.render_target_pool.get(draw_call_data.render_target - 1));
			else
				impl_->set_default_render_target();

			if (draw_call_data.command != nullptr)
			{
				if (!draw_call_data.command->execute())
					continue;
			}

			impl_->set_state(context.render_state_pool.get(draw_call_data.state));
		}
		else impl_->set_default_render_target();

		impl_->set_shader_program(context.shader_pool.get(material.shader_program));
		impl_->set_vertex_buffer(context.vertex_buffer_pool.get(node.mesh.vbuffer));
		impl_->set_index_buffer(context.index_buffer_pool.get(node.mesh.ibuffer));
		impl_->set_layout(context.layout_pool.get(node.mesh.layout));
		for (size_t i = 0; i < material_textures_number; ++i)
		{
			if (material.textures[i].id == Texture::invalid_id)
				continue;
			const Texture& texture = context.texture_pool.get(material.textures[i].id);
			impl_->set_texture(texture, i);
		}
		for (size_t i = 0; i < material_uniforms_number; ++i)
		{
			if (material.uniforms[i] == UniformBuffer::invalid_id)
				continue;
			const UniformBuffer& uniform = context.uniform_pool.get(material.uniforms[i]);
			impl_->set_uniform(uniform);
		}
		impl_->draw(node.mesh.render_data);
	}
}

}
