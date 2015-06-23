#include "render/video_driver.hpp"

#include "impl/system_factory.hpp"

#include "render/instances.hpp"
#include "render/context.hpp"
#include "render/draw_call.hpp"

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
	impl_(SystemFactory::instance().create_driver()),
	current_version_(0)
{
	reset_state();
	versions_number_ = impl_->supported_versions(versions_, ARRAY_SIZE(versions_));
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
	NOT_IMPLEMENTED(clear_stencil);
	NOT_IMPLEMENTED(stencil);
}

void Driver::reset()
{
	stats_.reset();
	impl_.reset(SystemFactory::instance().create_driver());
}

void Driver::render(const Context& context, const DrawCall* draw_calls, size_t count)
{
	ProfilerElement pe("driver.render");
	for (size_t i = 0; i < count; ++i)
		perform_draw_call(context, draw_calls[i]);
}

void Driver::render(const Context& context, const DrawCallExplicit* draw_calls, size_t count)
{
	ProfilerElement pe("driver.render_explicit");
	for (size_t i = 0; i < count; ++i)
		perform_draw_call(context, draw_calls[i]);
}

void Driver::perform_draw_call(const Context& context, const DrawCallExplicit& draw_call)
{
	if (draw_call.render_target != nullptr)
	{
		if (state_.render_target != draw_call.render_target->id())
		{
			state_.render_target = draw_call.render_target->id();
			impl_->set_render_target(*draw_call.render_target);
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

	if (draw_call.render_command != nullptr)
	{
		if (!draw_call.render_command->execute(render_stage_before_submit))
			return;
	}

	ASSERT(draw_call.render_state != nullptr, "Invalid RenderState");
	if (state_.state != draw_call.render_state->id())
	{
		impl_->set_state(*draw_call.render_state);
		state_.state = draw_call.render_state->id();
	}

	bool shader_program_changed = false;
	bool layout_changed = false;
	ASSERT(draw_call.shader_program != nullptr, "Invalid ShaderProgram");
	if (state_.shader_program != draw_call.shader_program->id())
	{
		impl_->set_shader_program(*draw_call.shader_program);
		state_.shader_program = draw_call.shader_program->id();
		shader_program_changed = true;
		layout_changed = true;
	}

	ASSERT(draw_call.vbuffer != nullptr, "Invalid VertexBuffer");
	if (state_.vertex_buffer != draw_call.vbuffer->id())
	{
		impl_->set_vertex_buffer(*draw_call.vbuffer);
		state_.vertex_buffer = draw_call.vbuffer->id();
		layout_changed = true;
	}

	ASSERT(draw_call.ibuffer != nullptr, "Invalid IndexBuffer");
	if (state_.index_buffer != draw_call.ibuffer->id())
	{
		impl_->set_index_buffer(*draw_call.ibuffer);
		state_.index_buffer = draw_call.ibuffer->id();
		layout_changed = true;
	}

	ASSERT(draw_call.layout != nullptr, "Invalid Layout");
	if (state_.layout != draw_call.layout->id() || layout_changed)
	{
		impl_->set_layout(*draw_call.layout);
		state_.layout = draw_call.layout->id();
	}

	for (size_t j = 0; j < material_textures_number; ++j)
	{
		if (draw_call.textures[j] == nullptr || draw_call.textures[j]->id() == state_.textures[j])
			continue;
		impl_->set_texture(*draw_call.textures[j], j);
		state_.textures[j] = draw_call.textures[j]->id();
	}
	for (size_t j = 0; j < material_uniforms_number; ++j)
	{
		if (draw_call.uniforms[j] == nullptr || (draw_call.uniforms[j]->id() == state_.uniforms[j] && !shader_program_changed))
			continue;
		impl_->set_uniform(*draw_call.uniforms[j], j);
		state_.uniforms[j] = draw_call.uniforms[j]->id();
	}
	impl_->draw(draw_call.elements_number, draw_call.vbuffer_offset, draw_call.ibuffer_offset, draw_call.indices_number, draw_call.primitive);
	stats_.update(draw_call.elements_number);
}

void Driver::perform_draw_call(const Context& context, const DrawCall& draw_call)
{
	uint8_t command_render_stages = draw_call.command != nullptr ? draw_call.command->stages() : 0;
	if (command_render_stages & render_stage_before_render_target_setup)
		draw_call.command->execute(render_stage_before_render_target_setup);

	if (context.draw_call_data_pool.is_valid(draw_call.draw_call_data))
	{
		const DrawCallData& draw_call_data = context.draw_call_data_pool.get(draw_call.draw_call_data);
		if (draw_call_data.render_target != default_render_target)
		{
			if (state_.render_target != draw_call_data.render_target)
			{
				state_.render_target = draw_call_data.render_target;
				impl_->set_render_target(context.render_target_pool.get(draw_call_data.render_target));
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

		if (state_.state != draw_call_data.state)
		{
			impl_->set_state(context.render_state_pool.get(draw_call_data.state));
			state_.state = draw_call_data.state;
		}

		if (command_render_stages & render_stage_before_submit)
		{
			if (!draw_call.command->execute(render_stage_before_submit))
				return;
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

	uint8_t material_system_id = draw_call.material.material_system;
	Material::IdType material_id = draw_call.material.id;
	const Material& material =
		context.materials[material_system_id].get(material_id);

	bool shader_program_changed = false;
	bool layout_changed = false;
	if (state_.shader_program != material.shader_program)
	{
		impl_->set_shader_program(context.shader_pool.get(material.shader_program));
		state_.shader_program = material.shader_program;
		shader_program_changed = true;
		layout_changed = true;
	}

	if (state_.vertex_buffer != draw_call.render_data.vbuffer)
	{
		impl_->set_vertex_buffer(context.vertex_buffer_pool.get(draw_call.render_data.vbuffer));
		state_.vertex_buffer = draw_call.render_data.vbuffer;
		layout_changed = true;
	}

	if (state_.index_buffer != draw_call.render_data.ibuffer)
	{
		impl_->set_index_buffer(context.index_buffer_pool.get(draw_call.render_data.ibuffer));
		state_.index_buffer = draw_call.render_data.ibuffer;
		layout_changed = true;
	}

	if (state_.layout != draw_call.render_data.layout || layout_changed)
	{
		impl_->set_layout(context.layout_pool.get(draw_call.render_data.layout));
		state_.layout = draw_call.render_data.layout;
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
		if (material.uniforms[j] == UniformBuffer::invalid_id || (material.uniforms[j] == state_.uniforms[j] && !shader_program_changed))
			continue;
		const UniformBuffer& uniform = context.uniform_pool.get(material.uniforms[j]);
		impl_->set_uniform(uniform, j);
		state_.uniforms[j] = material.uniforms[j];
	}
	impl_->draw(draw_call.render_data);
	stats_.update(draw_call.render_data.elements_number);

	if (command_render_stages & render_stage_after_submit)
		draw_call.command->execute(render_stage_after_submit);
}

void Driver::begin_render()
{
	impl_->set_default_render_target();
}

void Driver::end_render()
{
	stats_.update_frames();
	impl_->flush();
}

void Driver::reset_state()
{
	::memset(&state_, 0xff, sizeof(state_));
}

}
