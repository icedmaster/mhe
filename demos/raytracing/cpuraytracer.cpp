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

mhe::vec3 lit_pixel(const mhe::vec3& pos, const mhe::vec3& nrm, 
	const mhe::vec3& light_direction, const mhe::vec3& color)
{
	vec3 res = vec3::zero();
	float ndotl = dot(light_direction, nrm);
	if (ndotl >= 0.0f)
		res += color * ndotl;
	return res;
}

vec3 trace(game::Engine* engine, TraceContext& trace_context, const rayf& r, const mat4x4* transforms, const mat4x4* inv_transforms, size_t bounce)
{
	if (bounce >= max_bounces) return vec3::zero();

	Context& context = engine->context();
	NodeInstance* nodes = engine->scene_context().node_pool.all_objects();
	size_t nodes_number = engine->scene_context().node_pool.size();
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

		size_t lights_number = trace_context.lights_number;

		MaterialData material_data;
		context.material_manager.get(material_data, material_id);
		trace_context.shade_term = 0.0f;
		for (size_t i = 0; i < lights_number; ++i)
		{
			const vec3& light_direction = trace_context.lights_directions[i];

			vec3 ambient_color = vec3(0.1f, 0.1f, 0.1f);
			// direct lighting
			const vec3& direction = light_direction;
			const vec3& diffuse_color = trace_context.lights_diffuse_colors[i];
			rescolor = lit_pixel(res, nrm, direction, diffuse_color);

			TraceContext local_context;
			local_context.shade_term = 1.0f;
			local_context.node_id = node_id;
			local_context.pass = pass_shadow;
			rayf shadow_ray(res, -light_direction, trace_distance);
			trace(engine, local_context, shadow_ray, transforms, inv_transforms, 0);
			trace_context.emission = rescolor * material_data.render_data.glossiness / (bounce + 1);
			rescolor *= local_context.shade_term;

			for (size_t j = 0; j < hemisphere_reflection_rays_number / (bounce + 1); ++j)
			{
				vec3 random_dir = sample_hemisphere(random(0.0f, 1.0f), random(0.0f, 1.0f));
				rayf random_ray(res, tangent_space_to_world_space(random_dir, nrm), trace_distance);
				local_context = trace_context;
				local_context.node_id = node_id;
				local_context.emission = vec3::zero();
				local_context.pass = pass_default;
				local_context.shade_term = 1.0f;
				trace(engine, local_context, random_ray, transforms, inv_transforms, bounce + 1);
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

	size_t threads_number = thread::hardware_threads_number();
	for (size_t i = 0; i < threads_number; ++i)
	{
		TraceThread& thread = threads_.add();
		thread.start();
	}
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

	// lights
	size_t lights_number = engine_->scene_context().light_pool.size();
	LightInstance* lights = engine_->scene_context().light_pool.all_objects();
	std::vector<vec3> directions(lights_number);
	std::vector<vec3> diffuse_colors(lights_number);
	for (size_t i = 0; i < lights_number; ++i)
	{
		directions[i] = -get_light_direction(engine_->scene_context(), lights[i].id);
		diffuse_colors[i] = lights[i].light.shading().diffuse.xyz();
	}

	// real tracing
	std::vector<uint8_t> result(height * width * 4);

	size_t threads_number = 4; //thread::hardware_threads_number();
	size_t rows = context.window_system.height() / threads_number;
	for (size_t i = 0; i < threads_number; ++i)
	{
		ThreadConfig config;
		config.camera_position = camera.position();
		config.inv_vp = camera_data.inv_vp;
		config.data = &result[0];
		config.height = i != threads_number - 1 ? (i + 1) * rows : height;
		config.width = width;
		config.total_height = height;
		config.total_width = width;
		config.xs = 0;
		config.ys = i * rows;
		config.lights_directions = &directions[0];
		config.diffuse_colors = &diffuse_colors[0];
		config.lights_number = lights_number;
		threads_[i].init(engine_, &world[0], &inv_w[0], config);
	}

	{
		ProfilerElement pe("trace");
		for (size_t i = 0; i < threads_number; ++i)
			threads_[i].condition().notify();
		for (size_t i = 0; i < threads_number; ++i)
			threads_[i].result_condition().wait();
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

void CPURaytracer::TraceThread::init(mhe::game::Engine* engine, const mhe::mat4x4* transforms,
	const mhe::mat4x4* inv_transforms, const ThreadConfig& config)
{
	engine_ = engine;
	transforms_ = transforms;
	inv_transforms_ = inv_transforms;
	thread_config_ = config;
}
	
bool CPURaytracer::TraceThread::start_impl()
{
	return true;
}

void CPURaytracer::TraceThread::process_impl()
{
	cond_.wait();
	int ys = thread_config_.ys;
	int xs = thread_config_.xs;
	int height = thread_config_.height;
	int width = thread_config_.width;
	int total_width = thread_config_.total_width;
	int total_height = thread_config_.total_height;
	const mat4x4& inv_vp = thread_config_.inv_vp;
	const vec3& camera_pos = thread_config_.camera_position;
	const mat4x4* world = transforms_;
	const mat4x4* world_to_obj = inv_transforms_;
	uint8_t* result = thread_config_.data;
	vec3* directions = thread_config_.lights_directions;
	vec3* diffuse_colors = thread_config_.diffuse_colors;
	size_t lights_number = thread_config_.lights_number;
	for (int y = ys; y < height; ++y)
	{
		for (int x = xs; x < width; ++x)
		{
			vec4 ndc_pos(x * 2.0f / total_width - 1.0f, y * 2.0f / total_height - 1.0f, 1.0f, 1.0f);
			vec4 world_pos = ndc_pos * inv_vp;
			vec3 pos = world_pos.xyz() / world_pos.w();
			rayf r(camera_pos, pos);
			TraceContext trace_context;
			trace_context.node_id = NodeInstance::invalid_id;
			trace_context.shade_term = 1.0f;
			trace_context.pass = pass_default;
			trace_context.lights_directions = directions;
			trace_context.lights_diffuse_colors = diffuse_colors;
			trace_context.lights_number = lights_number;
			const vec3& pixel = saturate(::trace(engine_, trace_context, r, world, world_to_obj, 0));
			result[4 * (y * total_width + x) + 0] = pixel.x() * 255;
			result[4 * (y * total_width + x) + 1] = pixel.y() * 255;
			result[4 * (y * total_width + x) + 2] = pixel.z() * 255;
			result[4 * (y * total_width + x) + 3] = 255;
		}
	}
	res_cond_.notify();
}