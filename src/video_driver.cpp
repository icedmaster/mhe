#include "video_driver.hpp"

#include "renderable.hpp"
#include "shader_utils.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

// helper stats class
void Driver::Stats::update(const BatchedRenderable& renderable)
{
	tris_ += renderable.indicies().size() / 3;
	++batches_;
}

void Driver::Stats::update_frames()
{
	++frames_;
}

Driver::Driver() :
	impl_(SystemFactory::instance().create_driver()),
	batched_(initial_number_of_renderables)
{
	batched_.resize(0);	
}

void Driver::begin_render()
{
	renderable_elements_.clear();
    impl_->begin_render();
}

void Driver::end_render()
{	
	stats_.add_renderable_count(renderable_elements_.size());	
	perform_batch();
	for (size_t i = 0; i < batched_.size(); ++i)
		perform_render(batched_[i]);
    impl_->end_render();
	batched_.clear();
}

void Driver::begin_frame()
{
	stats_.update_frames();
}

void Driver::end_frame()
{}

void Driver::draw(Renderable* renderable)
{
	renderable_elements_.push_back(renderable);
}

void Driver::reset()
{
	stats_.reset();
	impl_.reset(SystemFactory::instance().create_driver());
}

void Driver::perform_batch()
{	
	Renderable* last_renderable = nullptr;
	for (renderable_container::const_iterator it = renderable_elements_.begin();
		 it != renderable_elements_.end(); ++it)
	{
		Renderable* renderable = *it;
        assert(renderable->material() != nullptr);
		if ( renderable->is_batching_disabled() || 
			 (last_renderable == nullptr) || !last_renderable->is_material_equals(*renderable)
			)
		{
			batched_container::iterator it = batched_.next_predefined_element();
			if (it == batched_.end())
				batched_.push_back(BatchedRenderable());
			last_renderable = renderable;
			BatchedRenderable& current = batched_.back();
			current.clear();
			for (size_t i = 0; i < last_renderable->materials_number(); ++i)
				current.add_material(last_renderable->material_at(i));
		}
		BatchedRenderable& current_batch = batched_.back();			
		current_batch.attach(*renderable);
	}	
}

void Driver::perform_render(const BatchedRenderable& renderable)
{
	set_render_flags(renderable);

	if (!impl_->support_buffered_render())
	{
		impl_->begin_draw(renderable.vertexcoord().data(),
						  renderable.normalscoord().data(), renderable.texcoord().data(),
						  renderable.colorcoord().data(), renderable.indicies().size());
		impl_->draw(renderable.indicies().data(), renderable.indicies().size());
		impl_->end_draw();
	}
	else perform_buffered_render(renderable);
	stats_.update(renderable);
	clear_render_flags(renderable);
}

void Driver::perform_buffered_render(const BatchedRenderable& renderable)
{
	boost::scoped_ptr<RenderBuffer> buffer(impl_->create_render_buffer());
	buffer->attach_data(vertex_position_attribute_name, renderable.vertexcoord().data(), renderable.vertexcoord().size());
	buffer->attach_data(vertex_normal_attribute_name, renderable.normalscoord().data(), renderable.normalscoord().size());
	buffer->attach_data(vertex_color_attribute_name, renderable.colorcoord().data(), renderable.colorcoord().size());
	for (size_t i = 0; i < renderable.materials_number(); ++i)
	{
		const std::string& name = utils::name_for_texture_attribute(i);
		const BatchedRenderable::texcoord_container& tc = renderable.texcoord_at(i);
		const float* data = tc.data();
		buffer->attach_data(name, data, tc.size());
	}
	if (!buffer->init()) return;
	buffer->enable();
	impl_->begin_draw(buffer.get(), renderable.materials(), renderable.materials_number());
	impl_->draw(renderable.indicies().data(), renderable.indicies().size());
	impl_->end_draw(buffer.get(), renderable.materials_number());
	buffer->disable();
}

void Driver::set_render_flags(const BatchedRenderable& renderable)
{
	if (renderable.is_z_buffer_masked())
		impl_->mask_zbuffer();
	if (!renderable.is_lighting_enabled())
		impl_->disable_lighting();
	if (renderable.is_blending_enabled())
	{
		impl_->enable_blending();
		impl_->set_blend_func(renderable.blend_mode());
	}
}

void Driver::clear_render_flags(const BatchedRenderable& renderable)
{
	if (renderable.is_z_buffer_masked())
		impl_->unmask_zbuffer();
	if (!renderable.is_lighting_enabled())
		impl_->enable_lighting();
	if (renderable.is_blending_enabled())
		impl_->disable_blending();
}

}
