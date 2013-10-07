#include "video_driver.hpp"

#include "renderable.hpp"
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
	boost::shared_ptr<Texture> last_texture;
	for (renderable_container::const_iterator it = renderable_elements_.begin();
		 it != renderable_elements_.end(); ++it)
	{
		Renderable* renderable = *it;
        assert(renderable->texture() != nullptr);
		if ( renderable->is_batching_disabled() || 
			 (last_texture == nullptr) || !last_texture->is_equal(*(renderable->texture()))
			)
		{
			batched_container::iterator it = batched_.next_predefined_element();
			if (it == batched_.end())
				batched_.push_back(BatchedRenderable(false));
			last_texture = renderable->texture();
			batched_.back().set_texture(last_texture);	
			batched_.back().clear();
		}
		BatchedRenderable& current_batch = batched_.back();			
		current_batch.attach(*renderable);
	}	
}

void Driver::perform_render(const BatchedRenderable& renderable)
{
	set_render_flags(renderable);
    renderable.texture()->prepare();

	if (!impl_->support_buffered_render())
	{
		impl_->begin_draw(renderable.vertexcoord().data(),
						  renderable.normalscoord().data(), renderable.texcoord().data(),
						  renderable.colorcoord().data(), renderable.indicies().size());
		impl_->draw(renderable.indicies().data(), renderable.indicies().size());
		impl_->end_draw();
	}
	else perform_buffered_render(renderable);
	renderable.texture()->clean();
	stats_.update(renderable);
	clear_render_flags(renderable);
}

void Driver::perform_buffered_render(const BatchedRenderable& renderable)
{
	boost::scoped_ptr<RenderBuffer> buffer(impl_->create_render_buffer());
	buffer->attach_data(vertex_position_attribute_name, renderable.vertexcoord().data(), renderable.vertexcoord().size());
	buffer->attach_data(vertex_normal_attribute_name, renderable.normalscoord().data(), renderable.normalscoord().size());
	buffer->attach_data(vertex_texcoord_attribute_name, renderable.texcoord().data(), renderable.texcoord().size());
	buffer->attach_data(vertex_color_attribute_name, renderable.colorcoord().data(), renderable.colorcoord().size());
	if (!buffer->init()) return;
	buffer->enable();
	impl_->begin_draw(buffer.get());
	impl_->draw(renderable.indicies().data(), renderable.indicies().size());
	impl_->end_draw(buffer.get());
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
