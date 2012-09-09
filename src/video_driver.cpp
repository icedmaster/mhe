#include "video_driver.hpp"

#include "renderable.hpp"

namespace mhe {

// helper stats class
void Driver::Stats::update(const Renderable& renderable)
{
	tris_ += renderable.indicies().size() / 3;
	++batches_;
}

void Driver::begin_render()
{
	renderable_elements_.clear();
}

void Driver::end_render()
{
	const std::vector<Renderable>& batched = perform_batch(renderable_elements_);
	for (size_t i = 0; i < batched.size(); ++i)
		perform_render(batched[i]);
}

void Driver::draw(Renderable* renderable)
{
	renderable_elements_.push_back(renderable);
}

std::vector<Renderable> Driver::perform_batch(const std::list<Renderable*>& elements) const
{
	boost::shared_ptr<Texture> last_texture;
	std::vector<Renderable> batches;
	batches.reserve(elements.size());
	for (std::list<Renderable*>::const_iterator it = elements.begin(); it != elements.end(); ++it)
	{
		Renderable* renderable = *it;
		if ( (last_texture == nullptr) || !last_texture->is_equal(*(renderable->texture())) )
		{
			batches.push_back(Renderable(false));
			last_texture = renderable->texture();
			batches.front().set_texture(last_texture);
			continue;
		}
		Renderable& current_batch = batches.front();			
		current_batch.attach(*renderable);
	}
	return batches;
}

void Driver::perform_render(const Renderable& renderable)
{
	begin_draw_impl(renderable.texture(), renderable.vertexcoord().data(),
					renderable.normalscoord().data(), renderable.texcoord().data(),
					renderable.color().get(), renderable.indicies().size());
	draw_impl(renderable.indicies().data(), renderable.indicies().size());
	end_draw_impl(renderable.texture());
	stats_.update(renderable);
}

}
