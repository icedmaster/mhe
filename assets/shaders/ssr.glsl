#define EARLY_EXIT

struct VSOutput
{
	vec2 tex;
};

#define FULLSCREEN_LAYOUT
[include "geometry_common.h"]

[vertex]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}

[fragment]

[include "common.h"]
[include "gbuffer_common.h"]

in VSOutput vsoutput;

out vec4 color;

vec4 ssr(vec3 ray_origin, vec3 ray_dir, GBuffer gbuffer, int samples, float initial_probe_distance, float max_error, float max_distance, float min_distance)
{
	vec3 origin = ray_origin;
	vec3 dir = ray_dir;
	vec3 reflected = reflect(ray_dir, gbuffer.normal);

#ifdef EARLY_EXIT
	if (dot(ray_dir, reflected) < 0.0f) return VEC4_ZERO;
	if (gbuffer.glossiness < 0.01f) return VEC4_ZERO;
#endif

	float fresnel = dot(-ray_dir, gbuffer.normal);

	vec4 res = VEC4_ZERO;
	vec2 res_pos;
	float weight = 0.0f;
	float probe_distance = initial_probe_distance;

	for (int i = 0; i < samples; ++i)
	{
		vec3 dst_pos = origin + reflected * probe_distance;
		vec4 dst_pos_proj = vp * vec4(dst_pos, 1.0f);
		vec3 dst_pos_ndc = dst_pos_proj.xyz / dst_pos_proj.w * 0.5f + 0.5f; // after perspective division is in [-1, 1] range + move to [0, 1]
		if (check_clamping(dst_pos_ndc.xy)) continue;
		float scene_depth = gbuffer_depth(dst_pos_ndc.xy); //[0, 1]
		float linear_scene_depth = linearized_depth(scene_depth, znear, zfar);
		float linear_pixel_depth = linearized_depth(dst_pos_ndc.z, znear, zfar);
		float err = linear_scene_depth - linear_pixel_depth;
		if (abs(err) < max_error)
		{
			res_pos = dst_pos_ndc.xy;
			weight = fresnel * (1.0f - abs(err));
		}
		vec3 probe_pos = position_from_depth(dst_pos_ndc.xy, scene_depth, inv_vp);
		probe_distance = clamp(length(probe_pos - origin), min_distance, max_distance);
	}

	return vec4(gbuffer_albedo(res_pos) * weight * gbuffer.glossiness, weight);
}

#define SSR_SAMPLES 10

void main()
{
	GBuffer gbuffer = gbuffer_unpack(vsoutput.tex);
	float pixel_depth = gbuffer.depth;
	vec3 world_pos = position_from_depth(vsoutput.tex, pixel_depth, inv_vp);
	vec3 camera_pos = viewpos.xyz;
	vec3 view_dir = normalize(world_pos - camera_pos);

	float max_distance = zfar;
	float min_distance = znear;
	float initial_probe_distance = (zfar - znear) / SSR_SAMPLES;
	float max_error = initial_probe_distance * 0.5f;
	
	vec4 ssr_pixel = ssr(world_pos, view_dir, gbuffer, SSR_SAMPLES, initial_probe_distance, max_error, max_distance, min_distance);
	color = vec4(ssr_pixel.rgb, 1.0f);
}