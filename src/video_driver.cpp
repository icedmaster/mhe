#include "video_driver.hpp"

#include "renderable.hpp"

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

void Driver::begin_render()
{
	renderable_elements_.clear();
	stats_.update_frames();
}

void Driver::end_render()
{	
	const std::vector<Renderable>& batched = perform_batch();
	for (size_t i = 0; i < batched.size(); ++i)
		perform_render(batched[i]);
}

void Driver::draw(Renderable* renderable)
{
	renderable_elements_.push_back(renderable);
}

std::vector<Renderable> Driver::perform_batch() const
{
	boost::shared_ptr<Texture> last_texture;
	std::vector<Renderable> batches;
	batches.reserve(renderable_elements_.size());
	for (std::list<Renderable*>::const_iterator it = renderable_elements_.begin();
		 it != renderable_elements_.end(); ++it)
	{
		Renderable* renderable = *it;
		if ( (last_texture == nullptr) || !last_texture->is_equal(*(renderable->texture())))
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

	begin_draw_impl(renderable.vertexcoord().data(),
					renderable.normalscoord().data(), renderable.texcoord().data(),
					renderable.colorcoord().data(), renderable.indicies().size());
	draw_impl(renderable.indicies().data(), renderable.indicies().size());
	end_draw_impl();
	renderable.texture()->clean();
	stats_.update(renderable);
	clear_render_flags(renderable);
}

void Driver::set_render_flags(const Renderable& renderable)
{
	if (renderable.is_z_buffer_masked())
		mask_zbuffer_impl();
	if (!renderable.is_lighting_enabled())
		disable_lighting_impl();
	if (renderable.is_blending_enabled())
	{
		enable_blending_impl();
		set_blend_func(renderable.blend_mode());
	}
}

void Driver::clear_render_flags(const Renderable& renderable)
{
	if (renderable.is_z_buffer_masked())
		unmask_zbuffer_impl();
	if (!renderable.is_lighting_enabled())
		enable_lighting_impl();
	if (renderable.is_blending_enabled())
		disable_blending_impl();
}

}
