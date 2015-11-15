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

	template <class T, size_t B>
	SH<T, B>& operator+= (const SH<T, B>& h)
	{
		for (size_t i = 0; i < coefficients_number; ++i)
			coeff[i] += h.coeff[i];
		return *this;
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
	const T cos_a1 = (T)mhe::pi;
	const T cos_a2 = (T)(2 * mhe::pi) / (T)3;
	const T cos_a3 = (T)(mhe::pi * (T)0.25);

	// band 0
	SHAssign<SHT, T>::assign(harmonics.coeff[0], 0.282095f * cos_a1);
	// band 1 (linear)
	SHAssign<SHT, T>::assign(harmonics.coeff[1], 0.488603f * dir.y() * cos_a2);
	SHAssign<SHT, T>::assign(harmonics.coeff[2], 0.488603f * dir.z() * cos_a2);
	SHAssign<SHT, T>::assign(harmonics.coeff[3], 0.488603f * dir.x() * cos_a2);
	// band2 (quadratic)
	SHAssign<SHT, T>::assign(harmonics.coeff[4], 1.092548f * dir.x() * dir.y() * cos_a3);
	SHAssign<SHT, T>::assign(harmonics.coeff[5], 1.092548f * dir.y() * dir.z() * cos_a3);
	SHAssign<SHT, T>::assign(harmonics.coeff[6], 0.315392f * (3 * dir.z() * dir.z() - 1) * cos_a3);
	SHAssign<SHT, T>::assign(harmonics.coeff[7], 1.092548f * dir.x() * dir.z() * cos_a3);
	SHAssign<SHT, T>::assign(harmonics.coeff[8], 0.546274f * (dir.x() * dir.x() - dir.y() * dir.y()) * cos_a3);
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

using namespace mhe;

class MeshBaker
{
	static const size_t texture_size = 64;

	struct DepthWriteShaderData
	{
		mat4x4 vp;
	};
public:
	bool init(mhe::Context& context, mhe::SceneContext& scene_context)
	{
		mhe::RenderTargetDesc render_target_desc;
		render_target_desc.width = texture_size;
		render_target_desc.height = texture_size;
		render_target_desc.depth_datatype = mhe::format_float;
		render_target_desc.depth_format = mhe::format_d24f;
		render_target_desc.use_depth = true;
		render_target_desc.use_stencil = false;
		render_target_desc.target = mhe::rt_readwrite;
		render_target_desc.texture_type = mhe::texture_2d;
		render_target_desc.color_targets = 1;
		render_target_desc.color_format[0] = mhe::format_rgba32f;
		render_target_desc.color_datatype[0] = mhe::format_default;
		for (size_t i = 0; i < 5; ++i)
		{
			mhe::RenderTarget& render_target = create_and_get(context.render_target_pool);
			if (!render_target.init(context, render_target_desc))
			{
				ERROR_LOG("Couldn't initialize a render target for MeshBaker");
				return false;
			}
			render_target_[i] = &render_target;
		}

		render_target_desc.color_targets = 0;
		render_target_desc.texture_type = mhe::texture_2d;
		for (size_t i = 0; i < 5; ++i)
		{
			mhe::RenderTarget& render_target_depth_only = create_and_get(context.render_target_pool);
			if (!render_target_depth_only.init(context, render_target_desc))
			{
				ERROR_LOG("Couldn't initialize a depth only render target for MeshBaker");
				return false;
			}
			render_target_depth_only_[i] = &render_target_depth_only;
		}

		// shader programs
		mhe::Shader shader;
		bool res = context.shader_manager.get(shader, mhe::string("depth_write"));
		ASSERT(res, "Can't get depth only shader");
		shader_program_depth_only_ = &context.shader_pool.get(context.ubershader_pool.get(shader.shader_program_handle).get_default());

		{
			UniformBuffer& uniform = create_and_get(context.uniform_pool);
			UniformBufferDesc uniform_buffer_desc;
			uniform_buffer_desc.size = sizeof(DepthWriteShaderData);
			uniform_buffer_desc.unit = 0;
			uniform_buffer_desc.update_type = uniform_buffer_normal;
			if (!uniform.init(uniform_buffer_desc))
			{
				ERROR_LOG("Can't init a uniform for MeshBaker");
				return false;
			}
			depth_write_uniform_ = &uniform;
		}

		context_ = &context;
		scene_context_ = &scene_context;

		clear_command_.set_driver(&context.driver);

		skybox_material_system_ = context.material_systems.get<mhe::SkyboxMaterialSystem>();
		ASSERT(skybox_material_system_ != nullptr, "SkyboxMaterialSystem isn't found");
		return true;
	}

	void destroy()
	{
	}

	void bake(mhe::TextureBuffer& texture_buffer, mhe::NodeInstance& node_instance)
	{
		ASSERT(node_instance.mesh.mesh.parts[0].render_data.layout == mhe::StandartGeometryLayout::handle, "Invalid layout");

		const VertexBuffer& vbuffer = context_->vertex_buffer_pool.get(node_instance.mesh.mesh.vbuffer);
		const IndexBuffer& ibuffer = context_->index_buffer_pool.get(node_instance.mesh.mesh.ibuffer);

		vector<StandartGeometryLayout::Vertex> vertices(vbuffer.size() / sizeof(StandartGeometryLayout::Vertex));
		vector<uint32_t> indices(ibuffer.size());

		vbuffer.data(reinterpret_cast<uint8_t*>(&vertices[0]), vertices.size() * sizeof(StandartGeometryLayout::Vertex));
		ibuffer.data(&indices[0], indices.size());

		mhe::vector<uint8_t> baked_data(indices.size() * sh::ColorSH::coefficients_number * sizeof(sh::ColorSH::datatype));
		float* data_ptr = reinterpret_cast<float*>(&baked_data[0]);

		mhe::DrawCallExplicit draw_call;

		for (size_t i = 0, size = vertices.size(); i < size; ++i)
		{
			const vec3& vertex_position = vertices[i].pos;
			const vec3& vertex_normal = vertices[i].nrm;
			vec3 x, y, z;
			z = vertex_normal;
			x = vertices[i].tng.xyz();
			y = cross(z, x) * vertices[i].tng.w();
			const sh::ColorSH& harmonics = render(0, draw_call, x, y, z, vertex_position, node_instance);
			memcpy(data_ptr, harmonics.coeff.data(), harmonics.coeff.size() * sizeof(sh::ColorSH::datatype));
			data_ptr += harmonics.coeff.size() * sizeof(sh::ColorSH::datatype) / sizeof(float);
		}

		texture_buffer.update(&baked_data[0]);
	}
private:
	sh::ColorSH render(size_t iteration, DrawCallExplicit& draw_call, const vec3& x, const vec3& y, const vec3& z, const vec3& pos, const mhe::NodeInstance& node_instance)
	{
		mat4x4 proj;
		proj.set_perspective(pi_2, 1.0f, 0.1f, 10.0f);

		NodeInstance* nodes = scene_context_->node_pool.all_objects();
		size_t nodes_number = scene_context_->node_pool.size();

		ViewportDesc viewport_desc;
		viewport_desc.viewport.set(0, 0, texture_size, texture_size);

		ViewportDesc viewport_desc_default;
		ScissorDesc scissor_desc_default;

		mat4x4 view[5];

		for (size_t i = 0; i < 5; ++i) // render a hemicube
		{
			prepare_draw_call(draw_call);
			view[i] = build_view_matrix(i, x, y, z, pos);
			const ScissorDesc& scissor_desc = get_scissor_desc(i);
			mat4x4 vp = view[i] * proj;
			if (iteration == 0) // depth only
			{
				DepthWriteShaderData shader_data;
				shader_data.vp = vp;
				depth_write_uniform_->update(shader_data);
				draw_call.uniforms[0] = depth_write_uniform_;

				draw_call.shader_program = shader_program_depth_only_;
				draw_call.render_target = render_target_depth_only_[i];
				draw_call.render_command = &clear_command_;

				clear_command_.reset();
			}
			else
			{
				NOT_IMPLEMENTED_ASSERT(0, "Everything");
			}

			for (size_t n = 0; n < nodes_number; ++n)
			{
				setup_draw_call(draw_call, nodes[n]);
				draw_call.render_state->update_viewport(viewport_desc);
				draw_call.render_state->update_scissor(scissor_desc);
				draw_call.uniforms[1]->update(mat4x4::identity());
				context_->driver.render(*context_, &draw_call, 1);
				// restore viewport and scissor settings
				draw_call.render_state->update_viewport(viewport_desc_default);
				draw_call.render_state->update_scissor(scissor_desc_default);

				context_->driver.reset_state();
			}

			{
				clear_command_.reset();

				mhe::RenderContext render_context;
				render_context.main_camera.vp = vp;
				skybox_material_system_->setup_draw_calls(*context_, *scene_context_, render_context);
				ASSERT(render_context.draw_calls.size() == 1, "Invalid number of draw calls has been gotten from SkyboxMaterialSystem");
				DrawCallExplicit skybox_draw_call;
				utils::convert(skybox_draw_call, render_context.draw_calls[0], *context_);
				skybox_draw_call.render_target = render_target_[i];
				skybox_draw_call.render_state->update_viewport(viewport_desc);
				skybox_draw_call.render_state->update_scissor(scissor_desc);
				skybox_draw_call.render_command = &clear_command_;
				context_->driver.render(*context_, &skybox_draw_call, 1);
				skybox_draw_call.render_state->update_viewport(viewport_desc_default);
				skybox_draw_call.render_state->update_scissor(scissor_desc_default);

				context_->driver.reset_state();
			}
		}

		return integrate(view, x, y, z);
	}

	void setup_draw_call(DrawCallExplicit& draw_call, const mhe::NodeInstance& node_instance) const
	{
		mhe::Material& material = context_->materials[node_instance.mesh.instance_parts[0].material.material_system].get(node_instance.mesh.instance_parts[0].material.id);
		mhe::DrawCallData& draw_call_data = context_->draw_call_data_pool.get(node_instance.mesh.instance_parts[0].draw_call_data);

		draw_call.elements_number = node_instance.mesh.mesh.parts[0].render_data.elements_number;
		draw_call.ibuffer = &context_->index_buffer_pool.get(node_instance.mesh.mesh.ibuffer);
		draw_call.ibuffer_offset = 0;
		draw_call.indices_number = node_instance.mesh.mesh.parts[0].render_data.indexes_number;
		draw_call.layout = &context_->layout_pool.get(node_instance.mesh.mesh.parts[0].render_data.layout);
		draw_call.pass = 0;
		draw_call.primitive = mhe::triangle;
		draw_call.priority = 0;
		draw_call.render_state = &context_->render_state_pool.get(draw_call_data.state);
		draw_call.textures[0] = &context_->texture_pool.get(material.textures[mhe::albedo_texture_unit].id);
		draw_call.uniforms[1] = &context_->uniform_pool.get(material.uniforms[1]);
		draw_call.vbuffer = &context_->vertex_buffer_pool.get(node_instance.mesh.mesh.vbuffer);
		draw_call.vbuffer_offset = 0;
	}

	mat4x4 build_view_matrix(int side, const vec3& x, const vec3& y, const vec3& z, const vec3& pos) const
	{
		mat4x4 view;
		vec3 view_side, view_up, view_fwd;
		switch (side)
		{
		case texture_posx:
			view_fwd = x;
			view_side = z;
			view_up = y;
			break;

		case texture_negx:
			view_fwd = -x;
			view_side = -z;
			view_up = y;
			break;

		case texture_posy:
			view_fwd = y;
			view_side = x;
			view_up = z;
			break;
			
		case texture_negy:
			view_fwd = -y;
			view_side = -x;
			view_up = z;
			break;

		case texture_posz:
			view_fwd = z;
			view_side = -x;
			view_up = y;
			break;
		default:
			ASSERT(0, "Invalid side for mesh baking");
			break;
		}

		view.set_column(0, view_side);
		view.set_column(1, view_up);
		view.set_column(2, -view_fwd);
		view.set_row(3, -pos);
		return view;
	}

	ScissorDesc get_scissor_desc(int side) const
	{
		ScissorDesc desc;
		desc.enabled = true;
		int w = texture_size, h = texture_size;
		int hw = texture_size / 2, hh = texture_size / 2;
		switch (side)
		{
		case texture_posx:
			desc.scissor_rect.set(0, 0, hw, h);
			break;

		case texture_negx:
			desc.scissor_rect.set(hw, 0, w, h);
			break;

		case texture_posy:
			desc.scissor_rect.set(0, 0, w, hh);
			break;

		case texture_negy:
			desc.scissor_rect.set(0, hh, w, h);
			break;

		default:
			desc.enabled = false;
			break;
		}

		return desc;
	}

	sh::ColorSH integrate(const mat4x4* view, const vec3& x, const vec3& y, const vec3& z)
	{
		sh::ColorSH res;

		mat4x4 ts;
		ts.set_column(0, x);
		ts.set_column(1, y);
		ts.set_column(2, z);

		float weight_total = 0.0f;
		vec4 data[texture_size * texture_size];
		for (size_t i = 0; i < 5; ++i)
		{
			mat4x4 view_to_world = view[i];
			view_to_world.set_row(3, vec3::zero());
			view_to_world.transpose();

			TextureInstance texture_instance;
			render_target_[i]->color_texture(texture_instance, 0);
			Texture& texture = context_->texture_pool.get(texture_instance.id);
			texture.read(reinterpret_cast<uint8_t*>(data), sizeof(data));

			for (size_t y = 0; y < texture_size; ++y)
			{
				for (size_t x = 0; x < texture_size; ++x)
				{
					float u = x / static_cast<float>(texture_size) * 2.0f - 1.0f;
					float v = y / static_cast<float>(texture_size) * 2.0f - 1.0f;
					vec3 dir_vs(u, v, -1.0f);
					dir_vs.normalize();

					vec3 dir_ws = dir_vs * view_to_world;
					vec3 dir_ts = dir_ws * ts;

					float weight = 1.0f + u * u + v * v;
					weight = 4.0f / (sqrt(weight) * weight);
					weight_total += weight;

					vec3 color = data[y * texture_size + x].rgb();
					color *= weight;
					res += sh::project_sh9(dir_ts, color);
				}
			}
		}

		float factor = 4 * pi * 0.5f / weight_total;
		return res * factor;
	}

	mhe::Context* context_;
	mhe::SceneContext* scene_context_;
	mhe::RenderTarget* render_target_[5];
	mhe::RenderTarget* render_target_depth_only_[5];
	mhe::ShaderProgram* shader_program_depth_only_;

	mhe::SkyboxMaterialSystem* skybox_material_system_;

	mhe::UniformBuffer* depth_write_uniform_;

	mhe::ClearCommand clear_command_;
};

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
		engine.context().mesh_manager.get_instance(node.mesh, mhe::string("sphere.bin"));
		node_ = &node;

		node.mesh.gi_data.texture_buffer = engine.context().texture_buffer_pool.create();
		mhe::TextureBuffer& texture_buffer = engine.context().texture_buffer_pool.get(node.mesh.gi_data.texture_buffer);

		mhe::setup_node(node, engine.context().material_systems.get<mhe::GBufferFillMaterialSystem>(), engine.context(), engine.scene_context(), mhe::string("white.tga"));

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

		renderer_ = static_cast<mhe::DeferredRenderer*>(engine.renderer());
		renderer_->disable();

		if (!mesh_baker.init(engine.context(), engine.scene_context()))
		{
			ERROR_LOG("MeshBaker initialization failed");
			return false;
		}

		for (size_t i = 0; i < 1; ++i)
		{
			//engine.context().driver.begin_render();
			mesh_baker.bake(texture_buffer, node);
			//engine.context().driver.end_render();
			//engine.context().window_system.swap_buffers();
		}

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

	// baker
	MeshBaker mesh_baker;
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
	camera_controller->set_move_speed(75.0f);
	camera_controller->set_rotation_speed(50.0f);
	app.engine().scene().set_camera_controller(camera_controller);

	return app.run();
}
