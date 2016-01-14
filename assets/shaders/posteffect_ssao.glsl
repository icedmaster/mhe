#define EARLY_EXIT
#define CHECK_NORMAL
//#define CALCULATE_NORMAL

[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]
[include "gbuffer_common.h"]

[uniform ssao_params 1 perframe]
uniform ssao_params
{
	vec4 ssaodata[3]; // 0: x - radius, y - power, z - min_distance, z - max_distance 1: x - fade start distance, y - AO max distance, z - intensity, w - samples number 2: x - rotation
};

[sampler2D noise_texture 4]

in VSOutput vsoutput;

out vec4 color;

vec3 random_over_hemisphere(vec2 u)
{
	float r = sqrt(abs(u.x));
	float theta = 2 * PI * u.y;
	return vec3(r * cos(theta), r * sin(theta), sqrt(max(0.0f, 1.0f - u.x)));
}

vec3 get_direction(vec2 noise, float index, mat3 tbn)
{
	vec2 u = vec2(random(noise, sin(index)), random(noise, cos(index)));
	vec3 rnd = random_over_hemisphere(u);
	return tbn * normalize(rnd);
}

mat3 create_tbn_matrix(vec3 normal, vec3 noise)
{
	vec3 tng = normalize(noise - normal * dot(normal, noise));
	vec3 bitng = cross(normal, tng);
	return mat3(tng, bitng, normal);
}


struct SSAOParameters
{
    int samples;
	float radius;
	float power;
	float occlusion_min_distance;
	float occlusion_max_distance;
	float fade_distance;
	float max_distance;
	float intensity;
    float rotation;
};

float ssao_probe(vec3 position, vec3 direction, vec3 normal, SSAOParameters params)
{
	vec3 probe_position_vs = position + direction * params.radius;	
	vec2 screen_position = position_to_screen_position(probe_position_vs, proj);

	float enabled = check_clamping(screen_position) ? 0.0f : 1.0f;

	// real scene depth
	float probe_nonlinear_depth = texture(depth_texture, screen_position).r;
	float probe_depth = linearized_depth(probe_nonlinear_depth, znear, zfar);
	// if scene depth is lesser it means that the probe point is occluded by something
#ifdef CHECK_NORMAL
	vec3 restored_probe_pos_vs = position_from_depth(screen_position, probe_nonlinear_depth, inv_proj);
	vec3 v = restored_probe_pos_vs - position;
	float dst = length(v);
	float weight = saturate(dot(normal, v / dst));
#else
	float dst = -probe_position_vs.z - probe_depth;
	float weight = 1.0f;
#endif

	weight *= saturate(dst / params.occlusion_min_distance);

	return saturate(1.0f - dst / params.occlusion_max_distance) * weight * enabled;
}

float ssao(vec2 uv, vec3 position, vec3 normal, SSAOParameters params)
{
	int probes = params.samples;
	float res = 0.0f;
	float weight = 1.0f / probes;

	ivec2 noise_texture_size = textureSize(noise_texture, 0);
	vec2 uv_coeff = viewport / noise_texture_size;

	// build a matrix to perform transition to the TBN space
	vec2 noise = texture(noise_texture, uv * uv_coeff).rg;
    float s = sin(params.rotation);
    float c = cos(params.rotation);
    mat2 rotation_matrix = mat2(
        s, c,
        -c, s);
    vec3 random_rotation = normalize(vec3(rotation_matrix * noise, 0.0f));
	mat3 tbn = create_tbn_matrix(normal, random_rotation);

	float depth_vs = -position.z;
	float fade = saturate((params.max_distance - depth_vs) / (params.max_distance - params.fade_distance));

#ifdef EARLY_EXIT
	if (fade < 0.01f) return 1.0f;
#endif

	for (int i = 0; i < probes; ++i)
	{
		vec3 direction = get_direction(uv, i, tbn);
		float probe_res = ssao_probe(position, direction, normal, params);
		res += probe_res;
	}

	return pow(1.0f - saturate(res * weight) * fade * params.intensity, params.power);
}

vec3 calculate_normal(vec3 pos)
{
	return normalize(cross(dFdx(pos), dFdy(pos)));
}

void main()
{
	SSAOParameters params;
	params.radius = ssaodata[0].x;
	params.power = ssaodata[0].y;
	params.occlusion_min_distance = ssaodata[0].z;
	params.occlusion_max_distance = ssaodata[0].w;
	params.fade_distance = ssaodata[1].x;
	params.max_distance = ssaodata[1].y;
	params.intensity = ssaodata[1].z;
    params.samples = int(ssaodata[1].w);
    params.rotation = ssaodata[2].x;
		
	GBuffer gbuffer = gbuffer_unpack(vsoutput.tex);
	float pixel_depth = gbuffer.depth;
	vec3 pos_vs = position_from_depth(vsoutput.tex, pixel_depth, inv_proj);

	mat3 world_to_view = mat3(view);

#ifndef CALCULATE_NORMAL
	vec3 normal_vs = world_to_view * gbuffer.normal;
#else
	vec3 normal_vs = calculate_normal(pos_vs);
#endif

	float ssao_value = ssao(vsoutput.tex, pos_vs, normal_vs, params);
	color = vec4(ssao_value, ssao_value, ssao_value, 1.0f);
}
