#include "mhe.hpp"

namespace sh
{

/*
template <class T>
T legendre(int l, int m, T x)
{
	static const T one = (T)1;
	T pmm = one;
	// P(mm) = (-1)^m * (2m - 1)!!(1 - x^2)^(m/2)
	T f = one; // for double-factorial part
	T s = sqrt((one - x) * (one + x)); // last part of the equation
	for (int i = 1; i <= m; ++i)
	{
		pmm *= -f * s;
		f += (T)2;
	}
	if (m == l) return pmm;
	// if l == m + 1
	// P(m, l) = x(2m + 1)P(m, m)
	if (l == m + 1)
	{
		return x * (2 * m + 1) * pmm;
	}
	// (l - m)P(m, l) = x(2l - 1)P(m, l - 1) - (l + m - 1)P(m, l - 2)
	T pll = 0;
	T p1 = x * (2 * m + 1) * pmm; // previous band
	T p2 = pmm; // pre-previous band
	for (int ll = m + 2; ll < l; ++ll) // 2 previous bands are used
	{
		pll = (x * (2 * ll - 1) * p1 - (ll + m - 1) * p2) / (ll - m);
		p2 = p1;
		p1 = pll;
	}

	return pll;
}
*/

int double_factorial(int n)
{
	if (n <= 1) return 1;
	return n * double_factorial(n - 2);
}

template <class T>
T legendre(int l, int m, T x)
{
	// I do not include Condon-Shortley phase here. Shader code contains only position coefficients 
	// as well as functions projecting directions onto SH space (for instance, project_sh9)
	if (l == m)
		return /*pow((T)-1, m) * */double_factorial(2 * m - 1) * pow(1 - x * x, m * (T)0.5);
	else if (l == m + 1)
		return x * (2 * m + 1) * legendre(m, m, x);
	else
		return (x * (2 * l - 1) * legendre(l - 1, m, x) - (l + m - 1) * legendre(l - 2, m, x)) / (l - m);
}

size_t factorial(int n)
{
	ASSERT(n >= 0, "Factorial is valid only for non-negative numbers");
	size_t res = 1;
	for (int i = 2; i <= n; ++i)
		res *= i;
	return res;
}

template <class T>
T K(int l, int m)
{
	return sqrt((T)(2 * l + 1) * factorial(l - abs(m)) / ((4 * mhe::pi) * factorial(l + abs(m))));
}

template <class T>
T cals_sh(int l, int m, T theta, T phi)
{
	if (m > 0) return sqrt(2.0f) * K<T>(l, m) * cos(m * phi) * legendre<T>(l, m, cos(theta));
	else if (m < 0) return sqrt(2.0f) * K<T>(l, m) * sin(-m * phi) * legendre<T>(l, -m, cos(theta));
	else return K<T>(l, 0) * legendre<T>(l, 0, cos(theta));
}

size_t sh_index(int l, int m)
{
	return l * (l + 1) + m;
}

template <class T, size_t B>
struct SH
{
	static const size_t bands = B;
	static const size_t coefficients_number = B * B;
	typedef T datatype;

	mhe::array<T, coefficients_number> coeff;

	SH()
	{
		coeff.fill(T());
	}

	void set(T theta, T phi)
	{
		for (int l = 0; l < bands; ++l)
		{
			for (int m = -l; m <= l; ++m)
			{
				coeff[sh_index(l, m)] = cals_sh(l, m, theta, phi);
			}
		}
	}
};

template <class T, size_t B, class U>
SH<T, B> operator* (const SH<T, B>& h, U c)
{
	SH<T, B> res;
	for (size_t i = 0; i < h.coeff.size(); ++i)
		res.coeff[i] = h.coeff[i] * c;
	return res;
}

template <class T>
struct Spherical
{
	T theta;
	T phi;
	T radius;

	Spherical() : theta(0), phi(0), radius(1) {}
	Spherical(T t, T p, T r) : theta(t), phi(p), radius(r) {}
};

template <class T>
mhe::vector3<T> spherical_to_corthesian(const Spherical<T>& s)
{
	float stheta = sin(s.theta);
	return mhe::vector3<T>(stheta * cos(s.phi), stheta * sin(s.phi), cos(s.theta)) * s.radius;
}

template <class T, size_t B>
struct Sample
{
	static const size_t bands = B;

	Spherical<T> sph;
	mhe::vector3<T> corth;
	SH<T, B> harmonics;
};

template <class T, class S>
SH<mhe::vector3<T>, S::bands> project(const S* samples, const mhe::vector3<T>* colors, size_t samples_number)
{
	const T weight = 4 * mhe::pi;
	const T norm_coeff = (T)1 / samples_number;

	typedef SH<mhe::vector3<T>, S::bands> OH;
	OH out_harmonics;

	const size_t coeff_number = OH::coefficients_number;

	for (size_t i = 0; i < samples_number; ++i)
	{
		for (size_t j = 0; j < coeff_number; ++j)
		{
			out_harmonics.coeff[j] += colors[i] * samples[i].harmonics.coeff[j];
		}
	}
	return out_harmonics * weight * norm_coeff;
}

template <class T, class V>
struct SHAssign
{
	static void assign(T& dst, const V& value)
	{
		dst = value;
	}
};

template <class V>
struct SHAssign< mhe::vector3<V>, V >
{
	static void assign(mhe::vector3<V>& dst, const V& value)
	{
		dst.set(value);
	}
};

template <class SHT, class T>
void project_sh9(SH<SHT, 3>& harmonics, const mhe::vector3<T>& dir)
{
	// band 0
	SHAssign<SHT, T>::assign(harmonics.coeff[0], 0.282095f);
	// band 1 (linear)
	SHAssign<SHT, T>::assign(harmonics.coeff[1], 0.488603f * dir.y());
	SHAssign<SHT, T>::assign(harmonics.coeff[2], 0.488603f * dir.z());
	SHAssign<SHT, T>::assign(harmonics.coeff[3], 0.488603f * dir.x());
	// band2 (quadratic)
	SHAssign<SHT, T>::assign(harmonics.coeff[4], 1.092548f * dir.x() * dir.y());
	SHAssign<SHT, T>::assign(harmonics.coeff[5], 1.092548f * dir.y() * dir.z());
	SHAssign<SHT, T>::assign(harmonics.coeff[6], 0.315392f * (3 * dir.z() * dir.z() - 1));
	SHAssign<SHT, T>::assign(harmonics.coeff[7], 1.092548f * dir.x() * dir.z());
	SHAssign<SHT, T>::assign(harmonics.coeff[8], 0.546274f * (dir.x() * dir.x() - dir.y() * dir.y()));
}

template <class T>
SH< mhe::vector3<T>, 3> project_sh9(const mhe::vector3<T>& dir, const mhe::vector3<T>& color)
{
	SH<mhe::vector3<T>, 3> res;
	project_sh9(res, dir);
	for (size_t i = 0; i < 9; ++i)
		res.coeff[i] = mhe::mul(res.coeff[i], color);
	return res;
}

const size_t test_bands_number = 3;
typedef Sample<float, test_bands_number> TestSample;
typedef SH<mhe::vec3, test_bands_number> ColorSH;

void generate_random_samples(TestSample* samples, size_t samples_number)
{
	float inv_samples_number = 1.0f / samples_number;
	size_t index = 0;
	for (size_t i = 0; i < samples_number; ++i)
	{
		for (size_t j = 0; j < samples_number; ++j)
		{
			float x = ((float)i + mhe::random(0.0f, 1.0f)) * inv_samples_number;
			float y = ((float)j + mhe::random(0.0f, 1.0f)) * inv_samples_number;
			samples[index].sph.theta = 2.0f * acos(sqrt(1.0f - x));
			samples[index].sph.phi = 2.0f * mhe::pi * y;
			samples[index].corth = spherical_to_corthesian(samples[index].sph);
			samples[index].harmonics.set(samples[index].sph.theta, samples[index].sph.phi);
			++index;
		}
	}
}

struct LightContext
{
	mhe::vec3 direction;
	mhe::colorf color;
};

ColorSH process_vertex(mhe::vec3* colors, const TestSample* samples, size_t samples_number, const mhe::vec3& vertex_normal, const LightContext& light_context)
{
	for (size_t i = 0; i < samples_number; ++i)
		colors[i] = light_context.color.rgb() * mhe::saturate(mhe::dot(light_context.direction, samples[i].corth));
	return project(samples, colors, samples_number);
}

} // sh namespace

class GameScene : public mhe::game::GameScene
{
	static const size_t test_samples_number = 100;
	static const size_t trace_samples_number = 64;
	static const size_t trace_distance = 100.0f;
public:
	bool init(mhe::game::Engine& engine, const mhe::game::GameSceneDesc& /*desc*/) override
	{
		init_render_data(engine);

		mhe::NodeInstance& node = engine.scene().create_node();
		mhe::utils::create_sphere(node, engine.context(), 3, mhe::utils::mesh_creation_flag_trace_data);
		node_ = &node;

		node.mesh.gi_data.texture_buffer = engine.context().texture_buffer_pool.create();
		mhe::TextureBuffer& texture_buffer = engine.context().texture_buffer_pool.get(node.mesh.gi_data.texture_buffer);

		mhe::setup_node(node, engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>(), engine.context(), engine.scene_context(), mhe::string("test.tga"));

		sh::generate_random_samples(samples_.data(), test_samples_number);

		mhe::MeshTraceDataInstance& trace_data = engine.context().mesh_trace_data_pool.get(node.mesh.mesh.trace_data_id);
		const mhe::vector<mhe::MeshGridTriangle>& triangles = trace_data.grid.data();

		// now we can init the texture buffer
		mhe::TextureBufferDesc desc;
		desc.update_type = mhe::buffer_update_type_static;
		desc.unit = 2;
		desc.format = mhe::format_r32f;
		texture_buffer.init(desc, triangles.size() * 3 * sh::ColorSH::coefficients_number * sizeof(sh::ColorSH::datatype), nullptr);
		texture_buffer_ = &texture_buffer;

		process_mesh(texture_buffer, triangles);

		renderer_ = static_cast<mhe::DeferredRenderer*>(engine.renderer());
		renderer_->disable();

		return true;
	}

	bool update(mhe::game::Engine& engine) override
	{
		return true;
	}

	void draw(mhe::game::Engine& engine) override
	{
		mhe::Context& context = engine.context();

		mhe::Material& material = context.materials[node_->mesh.instance_parts[0].material.material_system].get(node_->mesh.instance_parts[0].material.id);
		mhe::DrawCallData& draw_call_data = context.draw_call_data_pool.get(node_->mesh.instance_parts[0].draw_call_data);

		mhe::DrawCallExplicit draw_call;
		mhe::prepare_draw_call(draw_call);
		draw_call.elements_number = node_->mesh.mesh.parts[0].render_data.elements_number;
		draw_call.ibuffer = &context.index_buffer_pool.get(node_->mesh.mesh.ibuffer);
		draw_call.ibuffer_offset = 0;
		draw_call.indices_number = node_->mesh.mesh.parts[0].render_data.indexes_number;
		draw_call.layout = &context.layout_pool.get(node_->mesh.mesh.parts[0].render_data.layout);
		draw_call.pass = 0;
		draw_call.primitive = mhe::triangle;
		draw_call.priority = 0;
		draw_call.render_command = nullptr;
		draw_call.render_state = &context.render_state_pool.get(draw_call_data.state);
		draw_call.render_target = nullptr;
		draw_call.shader_program = default_program_;
		draw_call.textures[0] = &context.texture_pool.get(material.textures[mhe::albedo_texture_unit].id);
		draw_call.uniforms[0] = &context.uniform_pool.get(engine.render_context().main_camera.percamera_uniform);
		draw_call.uniforms[1] = &context.uniform_pool.get(material.uniforms[1]);
		draw_call.vbuffer = &context.vertex_buffer_pool.get(node_->mesh.mesh.vbuffer);
		draw_call.vbuffer_offset = 0;
		draw_call.texture_buffers[2] = texture_buffer_;

		context.driver.render(context, &draw_call, 1);
	}
private:
	void init_render_data(mhe::game::Engine& engine)
	{
		mhe::Context& context = engine.context();
		mhe::Shader shader;
		context.shader_manager.get(shader, mhe::string("12_sh"));
		ubershader_ = &context.ubershader_pool.get(shader.shader_program_handle);
		default_program_ = &context.shader_pool.get(ubershader_->get_default());
	}

	void process_mesh(mhe::TextureBuffer& texture_buffer, const mhe::vector<mhe::MeshGridTriangle>& triangles)
	{
		mhe::vector<uint8_t> baked_data(triangles.size() * 3 * sh::ColorSH::coefficients_number * sizeof(sh::ColorSH::datatype));
		baked_data.fill(0x5);
		float* data_ptr = reinterpret_cast<float*>(&baked_data[0]);
		sh::LightContext light_context;
		light_context.color = mhe::colorf(0.8f, 0.2f, 0.0f, 1.0f);
		light_context.direction = -mhe::vec3::right();

		int current_method = 1;

		// method 1 - just project directional light onto SH
		sh::ColorSH bkd_analytical = sh::project_sh9(-mhe::vec3::right(), mhe::vec3(0.8f, 0.2f, 0.0f));

		trace(light_context, triangles);

		for (size_t i = 0, size = triangles.size(); i < size; ++i)
		{
			for (size_t j = 0; j < 3; ++j)
			{
				// method 2 - project directional light, but using Monte-Carlo integration
				sh::ColorSH baked_lighting_sh = current_method == 0 ? bkd_analytical : process_vertex(color_samples_.data(), samples_.data(), samples_.size(), triangles[i].nrm[j], light_context);
				memcpy(data_ptr, baked_lighting_sh.coeff.data(), baked_lighting_sh.coeff.size() * sizeof(sh::ColorSH::datatype));
				data_ptr += baked_lighting_sh.coeff.size() * sizeof(sh::ColorSH::datatype) / sizeof(float);
			}
		}

		texture_buffer.update(&baked_data[0]);
	}

	struct TraceContext
	{
	};

	void trace(sh::LightContext& light_context, const mhe::vector<mhe::MeshGridTriangle>& triangles)
	{
		TraceContext trace_context;
		for (size_t i = 0, size = triangles.size(); i < size; ++i)
		{
			for (size_t j = 0; j < 3; ++j)
			{
				mhe::vec3 trace_dir_ts = mhe::random_sample_over_hemisphere(mhe::random(0.0f, 1.0f), mhe::random(0.0f, 1.0f));
				mhe::vec3 trace_dir_ws = mhe::tangent_space_to_world_space(trace_dir_ts, triangles[i].nrm[j]);
				mhe::rayf r(triangles[i].pos[j], trace_dir_ws, trace_distance);
				mhe::vec3 color = trace_step(trace_context, r, light_context);
			}
		}
	}

	mhe::vec3 trace_step(TraceContext& trace_context, const mhe::rayf& r, const sh::LightContext& light_context)
	{
		return mhe::vec3::zero();
	}

	mhe::NodeInstance* node_;

	typedef mhe::array<sh::TestSample, test_samples_number * test_samples_number> Samples;
	typedef mhe::array<mhe::vec3, test_samples_number * test_samples_number> ColorSamples;
	Samples samples_;
	ColorSamples color_samples_;

	mhe::DeferredRenderer* renderer_;

	// render data
	mhe::UberShader* ubershader_;
	mhe::ShaderProgram* default_program_;
	mhe::TextureBuffer* texture_buffer_;
};

int main(int /*argc*/, char** /*argv*/)
{
	mhe::app::Application app("12_sh");
	mhe::app::ApplicationConfig config;
	config.width = 1280;
	config.height = 720;
	config.bpp = 32;
	config.fullscreen = false;
#ifndef MHE_VS
	config.assets_path = "../assets/";
#else
	config.assets_path = "../../assets/";
#endif
	config.render_config_filename = mhe::utils::path_join(config.assets_path, "render_without_postprocess.xml");
	app.init(config);

	mhe::game::GameSceneDesc desc;
	GameScene* game_scene = new GameScene;
	game_scene->init(app.engine(), desc);
	app.engine().set_game_scene(game_scene);

	mhe::PerspectiveCameraParameters camera_parameters;
	camera_parameters.fov = 60.0f;
	camera_parameters.znear = 0.5f;
	camera_parameters.zfar = 20.0f;
	mhe::game::FPSCameraController* camera_controller = new mhe::game::FPSCameraController(app.engine(), camera_parameters,
		mhe::vec3(0, 1, 5), mhe::vec3(0.0f, mhe::pi, 0));
	camera_controller->set_move_speed(50.0f);
	app.engine().scene().set_camera_controller(camera_controller);

	return app.run();
}
