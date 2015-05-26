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

	std::vector<mat4x4> inv_w(engine_->scene_context().node_pool.size());
	NodeInstance* nodes = engine_->scene_context().node_pool.all_objects();
	for (size_t i = 0, size = inv_w.size(); i < size; ++i)
	{
		const Transform& transform = engine_->scene_context().transform_pool.get(nodes[i].transform_id).transform;
		inv_w[i] = transform.inverse_transform();
	}

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
				const vec3& pixel = trace(r, &inv_w[0]);
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

vec3 CPURaytracer::trace(const rayf& r, const mat4x4* inv_transforms)
{
	Context& context = engine_->context();
	NodeInstance* nodes = engine_->scene_context().node_pool.all_objects();
	size_t nodes_number = engine_->scene_context().node_pool.size();
	AABBInstance* aabbs = engine_->scene_context().parts_aabb_pool.all_objects();
	LightInstance* light_sources = engine_->scene_context().light_pool.all_objects();
	size_t lights_number = engine_->scene_context().light_pool.size();
	vec3 res;
	vec3 nrm;
	bool found = false;
	for (size_t i = 0; i < nodes_number; ++i)
	{
		vec3 input, output;
		if (!intersects(input, output, r, nodes[i].mesh.mesh.aabb))
			continue;
		MeshGrid& grid = context.mesh_trace_data_pool.get(nodes[i].mesh.mesh.trace_data_id).grid;
		MeshGridHelper helper(grid);
		if (helper.closest_intersection(res, nrm, r * inv_transforms[i]))
			found = true;
	}

	return found ? saturate(lit_pixel(res, nrm, vec3(0.2f, 0.2f, 0.2f), light_sources, lights_number)) : vec3::zero();
}

vec3 CPURaytracer::lit_pixel(const vec3& pos, const vec3& nrm, const vec3& ambient, const LightInstance* light_sources, size_t lights_number) const
{
	vec3 res = ambient;
	for (size_t i = 0; i < lights_number; ++i)
	{
		NOT_IMPLEMENTED_ASSERT(light_sources[i].light.type() == Light::directional, "Different light types");
		vec3 direction = -get_light_direction(engine_->scene_context(), light_sources[i].id);
		float ndotl = dot(direction, nrm);
		if (ndotl < 0.0f) continue;
		res += light_sources[i].light.shading().diffuse.xyz() * ndotl;
	}
	return res;
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