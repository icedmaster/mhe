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

	{
		ProfilerElement pe("trace");
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				vec4 ndc_pos(x * 2.0f / width - 1.0f, y * 2.0f / height - 1.0f, 1.0f, 1.0f);
				vec4 world_pos = ndc_pos * camera_data.inv_vp;
				vec3 pos = world_pos.xyz() / world_pos.w();
				rayf r(camera.position(), pos);
				const vec3& pixel = trace(r);
				result[4 * (y * width + x) + 0] = pixel.x() * 255;
				result[4 * (y * width + x) + 1] = pixel.y() * 255;
				result[4 * (y * width + x) + 2] = pixel.z() * 255;
				result[4 * (y * width + x) + 3] = 255;
			}
		}
		pe.stop();
		std::cout << "trace:" << pe.result() << std::endl;
	}

	{
		ProfilerElement pe("texture_update");
		texture_->update(static_cast<const uint8_t*>(&result[0]));
		pe.stop();
		std::cout << "texture_update:" << pe.result() << std::endl;
	}
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
		MeshGrid& grid = context.mesh_trace_data_pool.get(nodes[i].mesh.mesh.trace_data_id).grid;
		MeshGridHelper helper(grid);
		vec3 res;
		if (helper.closest_intersection(res, r))
			return vec3(1.0f, 1.0f, 1.0f);
	}

	return vec3::zero();
}

void CPURaytracer::kick_draw_call()
{
	Context& context = engine_->context();
	RenderContext local_context;
	PosteffectSimpleMaterialSystem* material_system = context.material_systems.get<PosteffectSimpleMaterialSystem>();
	ASSERT(material_system, "Invalid MaterialSystem");
	material_system->enable();
	TextureInstance texture_instance;
	texture_instance.id = texture_->id();
	material_system->set_texture(0, texture_instance);
	material_system->setup_draw_calls(context, engine_->scene_context(), local_context);
	ASSERT(local_context.draw_calls.size() == 1, "DrawCall setup failed");
	context.driver.reset_state();
	context.driver.render(context, local_context.draw_calls.data(), 1);
	material_system->disable();
}