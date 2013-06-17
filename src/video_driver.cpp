#include "video_driver.hpp"

#include "renderable.hpp"
#include "impl/system_factory.hpp"

namespace mhe {

// helper stats class
void Driver::Stats::update(const Renderable& renderable)
{
	tris_ += renderable.indicies().size() / 3;
	++batches_;
}

void Driver::Stats::update_frames()
{
	++frames_;
}

Driver::Driver() :
	impl_(SystemFactory::instance().create_driver())
{}

void Driver::begin_render()
{
	renderable_elements_.clear();
    impl_->begin_render();
}

void Driver::end_render()
{	
	stats_.add_renderable_count(renderable_elements_.size());
	const batched_container& batched = perform_batch();
	for (size_t i = 0; i < batched.size(); ++i)
		perform_render(batched[i]);
    impl_->end_render();
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

Driver::batched_container Driver::perform_batch() const
{
	boost::shared_ptr<Texture> last_texture;
	batched_container batches;
	for (renderable_container::const_iterator it = renderable_elements_.begin();
		 it != renderable_elements_.end(); ++it)
	{
		Renderable* renderable = *it;
        assert(renderable->texture() != nullptr);
		if ( renderable->is_batching_disabled() || 
			 (last_texture == nullptr) || !last_texture->is_equal(*(renderable->texture()))
			)
		{
			batches.push_back(Renderable(false));
			last_texture = renderable->texture();
			batches.back().set_texture(last_texture);
		}
		Renderable& current_batch = batches.back();			
		current_batch.attach(*renderable);
	}
	return batches;
}

void Driver::perform_render(const Renderable& renderable)
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

void Driver::perform_buffered_render(const Renderable& renderable)
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

void Driver::set_render_flags(const Renderable& renderable)
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

void Driver::clear_render_flags(const Renderable& renderable)
{
	if (renderable.is_z_buffer_masked())
		impl_->unmask_zbuffer();
	if (!renderable.is_lighting_enabled())
		impl_->enable_lighting();
	if (renderable.is_blending_enabled())
		impl_->disable_blending();
}

}
