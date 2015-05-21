#include "cpuraytracer.hpp"
#include <mhe.hpp>

using namespace mhe;

void CPURaytracer::init(mhe::game::Engine* engine)
{
	engine_ = engine;

	Context& context = engine->context();
	TextureDesc texture_desc;
	texture_desc.height = context.window_system.height();
	texture_desc.width = context.window_system.width();
	texture_desc.address_mode_s = texture_clamp;
	texture_desc.address_mode_t = texture_clamp;
	texture_desc.datatype = format_ubyte;
	texture_desc.format = format_rgba;
	texture_desc.mips = 0;
	texture_desc.type = texture_2d;

	Texture& texture = create_and_get(context.texture_pool);
	texture.init(texture_desc, nullptr, 0);
	texture_ = &texture;
}

void CPURaytracer::render(const Camera& camera)
{
	Context& context = engine_->context();
	int width = context.window_system.width();
	int height = context.window_system.height();

	CameraData camera_data;
	camera.get(camera_data);

	std::vector<uint8_t> result(height * width * 4);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			vec3 screen_pos(x / width * 2.0f - 1.0f, y / height * 2.0f - 1.0f, 1.0f);
			vec3 world_pos = screen_pos * camera_data.inv_vp;
			rayf r(camera.position(), world_pos);
			const vec3& pixel = trace(r);
			result[y * width + x * 4 + 0] = pixel.x() * 255;
			result[y * width + x * 4 + 1] = pixel.y() * 255;
			result[y * width + x * 4 + 2] = pixel.z() * 255;
			result[y * width + x * 4 + 3] = 255;
		}
	}

	texture_->update(static_cast<const uint8_t*>(&result[0]));
	kick_draw_call();
}

vec3 CPURaytracer::trace(const rayf& r)
{
	Context& context = engine_->context();
	NodeInstance* nodes = engine_->scene_context().node_pool.all_objects();
	size_t nodes_number = engine_->scene_context().node_pool.size();
	AABBInstance* aabbs = engine_->scene_context().parts_aabb_pool.all_objects();
	for (size_t i = 0; i < nodes_number; ++i)
	{
		for (size_t j = 0, size = nodes[i].mesh.instance_parts.size(); j < size; ++j)
		{
			const AABBInstance& aabb = aabbs[nodes[i].mesh.instance_parts[j].aabb_id];
			vec3 input, output;
			if (!intersects(input, output, r, aabb.aabb))
				continue;
		}
	}

	return vec3::zero();
}

void CPURaytracer::kick_draw_call()
{

}