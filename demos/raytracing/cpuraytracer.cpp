#include "cpuraytracer.hpp"
#include <mhe.hpp>

using namespace mhe;

const size_t max_bounces = 2;
const float trace_distance = 50.0f;
const size_t hemisphere_reflection_rays_number = 256;

//http://www.rorydriscoll.com/2009/01/07/better-sampling/
vec3 sample_hemisphere(float u1, float u2)
{
	const float r = sqrt(u1);
	const float theta = 2 * pi * u2;

	const float x = r * cosf(theta);
	const float y = r * sinf(theta);

	return vec3(x, y, sqrt(std::max(0.0f, 1 - u1)));
}

vec3 least_important_direction(const vec3& v)
{
	vec3 abs_v = abs(v);
	if (abs_v.x() < abs_v.y())
		return abs_v.x() < abs_v.z() ? vec3::right() : vec3::forward();
	else
		return abs_v.y() < abs_v.z() ? vec3::up() : vec3::forward();
}

vec3 perpendicular(const vec3& v)
{
	return cross(least_important_direction(v), v);
}

vec3 tangent_space_to_world_space(const vec3& v, const vec3& nrm)
{
	const vec3& t1 = perpendicular(nrm);
	const vec3& t2 = cross(nrm, t1);
	return (v.x() * t1 + v.y() * t2 + v.z() * nrm).normalized();
}

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
	std::vector<mat4x4> world(engine_->scene_context().node_pool.size());
	NodeInstance* nodes = engine_->scene_context().node_pool.all_objects();
	for (size_t i = 0, size = inv_w.size(); i < size; ++i)
	{
		const Transform& transform = engine_->scene_context().transform_pool.get(nodes[i].transform_id).transform;
		world[i] = transform.transform();
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
				TraceContext trace_context;
				trace_context.node_id = NodeInstance::invalid_id;
				trace_context.shade_term = 1.0f;
				trace_context.pass = pass_default;
				const vec3& pixel = saturate(trace(trace_context, r, &world[0], &inv_w[0], 0));
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

vec3 CPURaytracer::trace(TraceContext& trace_context, const rayf& r, const mat4x4* transforms, const mat4x4* inv_transforms, size_t bounce)
{
	if (bounce >= max_bounces) return vec3::zero();

	Context& context = engine_->context();
	NodeInstance* nodes = engine_->scene_context().node_pool.all_objects();
	size_t nodes_number = engine_->scene_context().node_pool.size();
	vec3 res;
	vec3 nrm;
	float dist = float_max;
	bool found = false;
	MaterialId material_id;
	size_t index;
	NodeInstance::IdType node_id = NodeInstance::invalid_id;

	for (size_t i = 0; i < nodes_number; ++i)
	{
		if (nodes[i].id == trace_context.node_id) continue;
		rayf localspace_ray = r * inv_transforms[i];
		vec3 input, output;
		if (!intersects(input, output, localspace_ray, nodes[i].mesh.mesh.aabb))
			continue;
		MeshGrid& grid = context.mesh_trace_data_pool.get(nodes[i].mesh.mesh.trace_data_id).grid;
		hitf obj_hit;
		MeshGridHelper helper(grid);
		if (helper.closest_intersection(obj_hit, localspace_ray))
		{
			found = true;
			if (trace_context.pass == pass_shadow)
			{
				trace_context.shade_term = 0.0f;
				return vec3::zero();
			}
			if (obj_hit.distance < dist)
			{
				res = obj_hit.point;
				nrm = obj_hit.normal;
				dist = obj_hit.distance;
				material_id = nodes[i].mesh.mesh.parts[0].material_id;
				index = i;
				node_id = nodes[i].id;
			}
		}
	}

	if (trace_context.pass == pass_shadow)
		return vec3::zero();

	vec3 rescolor = vec3::zero();
	if (found)
	{
		res = res * transforms[index];
		mat4x4 nrm_transform = transforms[index];
		nrm_transform.set_row(3, vec4::zero()); // consider that we're using uniform scale. Actually should be N = transpose(inverse(W))
		nrm = nrm * nrm_transform;
		nrm.normalize();

		vec3 emission_term;

		LightInstance* light_sources = engine_->scene_context().light_pool.all_objects();
		size_t lights_number = engine_->scene_context().light_pool.size();

		MaterialData material_data;
		context.material_manager.get(material_data, material_id);
		trace_context.shade_term = 0.0f;
		for (size_t i = 0; i < lights_number; ++i)
		{
			const vec3& light_direction = get_light_direction(engine_->scene_context(), light_sources[i].id);

			vec3 ambient_color = vec3(0.1f, 0.1f, 0.1f);
			// direct lighting
			vec3 direction = light_direction;
			vec3 diffuse_color = light_sources[i].light.shading().diffuse.xyz();
			rescolor = lit_pixel(res, nrm, direction, diffuse_color);

			TraceContext local_context;
			local_context.shade_term = 1.0f;
			local_context.node_id = node_id;
			local_context.pass = pass_shadow;
			rayf shadow_ray(res, -light_direction, trace_distance);
			trace(local_context, shadow_ray, transforms, inv_transforms, 0);
			trace_context.emission = rescolor * material_data.render_data.glossiness / (bounce + 1);
			rescolor *= local_context.shade_term;

			for (size_t j = 0; j < hemisphere_reflection_rays_number / (bounce + 1); ++j)
			{
				vec3 random_dir = sample_hemisphere(random(0.0f, 1.0f), random(0.0f, 1.0f));
				rayf random_ray(res, tangent_space_to_world_space(random_dir, nrm), trace_distance);
				local_context.node_id = node_id;
				local_context.emission = vec3::zero();
				local_context.pass = pass_default;
				local_context.shade_term = 1.0f;
				trace(local_context, random_ray, transforms, inv_transforms, bounce + 1);
				ASSERT(local_context.emission.x() >= 0.0f && local_context.emission.y() >= 0.0f && local_context.emission.z() >= 0.0f, "Invalid color");
				rescolor += local_context.emission;
				emission_term += local_context.emission;
			}
			//rescolor += ambient_color;
			//rescolor = nrm.normalized();
			//rescolor = emission_term;
		}
	}

	return rescolor;
}

vec3 CPURaytracer::lit_pixel(const vec3& pos, const vec3& nrm, const mhe::LightInstance& light_instance, const vec3& light_direction) const
{
	NOT_IMPLEMENTED_ASSERT(light_instance.light.type() == Light::directional, "Different light types");
	return lit_pixel(pos, nrm, light_direction, light_instance.light.shading().diffuse.xyz());
}

mhe::vec3 CPURaytracer::lit_pixel(const mhe::vec3& pos, const mhe::vec3& nrm, 
	const mhe::vec3& light_direction, const mhe::vec3& color) const
{
	vec3 res = vec3::zero();
	vec3 direction = -light_direction;
	float ndotl = dot(direction, nrm);
	if (ndotl >= 0.0f)
		res += color * ndotl;
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