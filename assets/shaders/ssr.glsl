#define EARLY_EXIT
//#define VIEWSPACE

[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]
[include "gbuffer_pbr_common.h"]

[uniform ssr_params 1 per_frame]
uniform ssr_params
{
	vec4 ssr_data[3];
};

[sampler2D lit_texture 4]

in VSOutput vsoutput;

out vec4 color;

struct SSRParams
{
	float min_distance;
	float max_distance;
	float max_error;
	float start_distance;
	float end_distance;
	float intensity;
	float fade_distance;

	float uv_fade;
};

vec4 ssr(vec3 ray_origin, vec3 ray_dir, GBuffer gbuffer, int samples, float initial_probe_distance, SSRParams params)
{
	vec3 origin = ray_origin;
	vec3 dir = ray_dir;

	mat3 world_to_view = mat3(view);
	vec3 normal_vs = world_to_view * gbuffer.normal;
	
	float current_depth = linearized_depth(gbuffer.depth, znear, zfar);

	vec3 reflected = reflect(ray_dir, normal_vs);

#ifdef EARLY_EXIT
	if (dot(ray_dir, reflected) < 0.0f) return VEC4_ZERO;
	if (gbuffer.metalness < 0.01f) return VEC4_ZERO;
#endif

	float fade_distance = params.fade_distance;
	float start_distance_weight = saturate((current_depth - params.start_distance) / fade_distance);
	float end_distance_weight = saturate((params.end_distance - current_depth) / fade_distance);
	float distance_weight = start_distance_weight * end_distance_weight;

	float view_weight = saturate(dot(dir, vec3(0, 0, -1)));

	vec2 res_pos;
	float weight = 0.0f;
	float probe_distance = initial_probe_distance;
	float prev_error = params.max_error;

	float uv_fade = 1.0f - params.uv_fade;

	for (int i = 0; i < samples; ++i)
	{
		vec3 dst_pos = origin + reflected * probe_distance;
		vec4 dst_pos_proj = proj * vec4(dst_pos, 1.0f);
		vec3 dst_pos_ndc = dst_pos_proj.xyz / dst_pos_proj.w * 0.5f + 0.5f;
		bool skip = check_clamping(dst_pos_ndc.xy);
		if (skip)
		{
			probe_distance *= 0.5f;
			continue;	
		}
		float scene_depth = gbuffer_depth(dst_pos_ndc.xy); //[0, 1]
		float linear_pixel_depth = dst_pos.z;
		vec3 probe_pos_vs = position_from_depth(dst_pos_ndc.xy, scene_depth, inv_proj);
		float linear_scene_depth = probe_pos_vs.z;
		float err = length(probe_pos_vs - dst_pos);
		float sign = linear_pixel_depth - linear_scene_depth > 0.0f ? +1.0f : -1.0f;
		float dist = length(probe_pos_vs - origin);
		if (err < prev_error && dist >= params.min_distance && dist <= params.max_distance)
		{
			res_pos = dst_pos_ndc.xy;
			vec2 res_pos_norm = abs(res_pos.xy * 2.0f - 1.0f);
			float uvmax = max(res_pos_norm.x, res_pos_norm.y);
			float uv_weight = saturate((params.uv_fade - uvmax) / uv_fade);
			weight = (1.0f - err / params.max_error) * uv_weight;
			prev_error = err;
		}
		probe_distance = probe_distance + err * sign;
	}

	vec3 lit = texture(lit_texture, res_pos).rgb;

	lit *= weight * view_weight;

	return vec4(lit * gbuffer.metalness * params.intensity * distance_weight, weight);
}

void main()
{
	GBuffer gbuffer = gbuffer_unpack(vsoutput.tex);
	float pixel_depth = gbuffer.depth;
	vec3 pos_vs = position_from_depth(vsoutput.tex, pixel_depth, inv_proj);
	vec3 camera_pos = VEC3_ZERO;
	vec3 view_dir = normalize(pos_vs);

	int samples = int(ssr_data[0].x);

	SSRParams params;
	params.max_distance = zfar;
	params.min_distance = ssr_data[0].z;
	params.max_error = ssr_data[0].w;
	params.start_distance = ssr_data[1].x;
	params.end_distance = ssr_data[1].y;
	params.intensity = ssr_data[1].z;
	params.fade_distance = ssr_data[1].w;
	params.uv_fade = ssr_data[2].x;

	float initial_probe_distance = (zfar - params.min_distance) / samples;
	
	vec4 ssr_pixel = ssr(pos_vs, view_dir, gbuffer, samples, initial_probe_distance, params);
	color = vec4(ssr_pixel.rgb, 1.0f);
}
