[defs LIGHT_TYPE 0 2]
[defs SHADOWMAP 0 1]
[defs SHADOWMAP_QUALITY 0 2]
[defs CUBEMAP 0 1]

#define LIGHT_DIRECTION_FROM_SOURCE
#define CASCADED_SHADOWMAP
//#define SHADOWMAP_DISC_FILTER

[include "common.h"]
[include "lighting_common.h"]

#ifdef DIRECTIONAL_CSM
//#define CASCADED_SHADOWMAP_DEBUG
#endif

// Define layout before "geometry_common.h" will be included
// TODO: SPOT_LIGHT is temporary here
#if LIGHT_TYPE == DIRECTIONAL_LIGHT || LIGHT_TYPE == SPOT_LIGHT
#define FULLSCREEN_LAYOUT
#else
#define DEBUG_LAYOUT
#endif

[include "geometry_common.h"]

uniform lights
{
	Light light;
};

#ifndef FULLSCREEN_LAYOUT
struct VSOutput
{
	vec4 pos;
};
#else
struct VSOutput
{
	vec2 tex;
};
#endif

[vertex]

out VSOutput vsoutput;

#ifndef FULLSCREEN_LAYOUT
void main()
{
	vec4 out_pos = vp * light.lightw * vec4(pos, 1.0f);
	vsoutput.pos = out_pos;
	gl_Position = out_pos;
}
#else
void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}
#endif

[fragment]

[sampler2D albedo_texture 0]
[sampler2D normal_texture 1]
[sampler2D depth_texture 2]
#if SHADOWMAP == 1
[sampler2D shadowmap_texture 5]
#endif

#if CUBEMAP == 1
[samplerCube env_cubemap 6]
#endif

in VSOutput vsoutput;

out vec4 color;

#if SHADOWMAP == 1

#if SHADOWMAP_QUALITY == 0
#define PCF_TAPS 0
#elif SHADOWMAP_QUALITY == 1
#define PCF_TAPS 2
#elif SHADOWMAP_QUALITY == 2
#define PCF_TAPS 3
#endif

#define PCF_SIZE (PCF_TAPS * 2 + 1)

#if SHADOWMAP_QUALITY == 0
#define DISC_TAPS 0
#elif SHADOWMAP_QUALITY == 1
#define DISC_TAPS 8
#elif SHADOWMAP_QUALITY == 2
#define DISC_TAPS 16
#endif

[include "pcf_kernels.h"]
[include "poisson_disc_kernel.h"]

#pragma optionNV (unroll all)

float pcf_filter(sampler2D tex, float pixel_depth, vec2 texcoord, float bias)
{
	float shadow_value = 0.0f;
	float weight = 0.0f;
	for (int y = -PCF_TAPS; y <= PCF_TAPS; ++y)
	{
		for (int x = -PCF_TAPS; x <= PCF_TAPS; ++x)
		{
			float shadowmap_depth = textureOffset(tex, texcoord, ivec2(x, y)).x;
			float tap_weight = pcf_weights[(y + PCF_TAPS) * PCF_SIZE + x + PCF_TAPS];
			shadow_value += pixel_depth < shadowmap_depth + bias ? 1.0f * tap_weight : 0.0f;
			weight += tap_weight;
		}
	}
		
	return saturate(shadow_value / weight);
}

float disc_filter(sampler2D tex, float pixel_depth, vec2 texcoord, float bias)
{
	vec2 inv_tex_size = 1.0f / textureSize(tex, 0);
	float weight = DISC_TAPS;
	float shadow_value = 0.0f;
	for (int i = 0; i < DISC_TAPS; ++i)
	{
		vec2 offset = disc_kernel[i] * inv_tex_size;
		float shadowmap_depth = texture(tex, texcoord + offset).x;
		shadow_value += pixel_depth < shadowmap_depth + bias ? 1.0f : 0.0f;
	}

	return shadow_value / weight;
}

float get_shadow_value(sampler2D tex, float pixel_depth, vec2 texcoord, float bias)
{
#if SHADOWMAP_QUALITY != 0
	#ifndef SHADOWMAP_DISC_FILTER
	return pcf_filter(tex, pixel_depth, texcoord, bias);
	#else
	return disc_filter(tex, pixel_depth, texcoord, bias);
	#endif
#else
	return pixel_depth < texture(tex, texcoord).r + bias ? 1.0f : 0.0f;
#endif
}

#endif	// SHADOWMAP

#ifdef CASCADED_SHADOWMAP_DEBUG
const vec3 cascade_color[MAX_CASCADES_NUMBER] = vec3[MAX_CASCADES_NUMBER]
(
	vec3(1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(1.0f, 1.0f, 0.0f),
	vec3(1.0f, 0.0f, 1.0f),
	vec3(0.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f)
);
#endif

void main()
{
	// TODO: add pack and unpack methods
#ifndef FULLSCREEN_LAYOUT
	vec2 tex = vsoutput.pos.xy / vsoutput.pos.w * 0.5f + 0.5f;
#else
	vec2 tex = vsoutput.tex;
#endif
	vec4 normal_data = texture(normal_texture, tex);
	vec3 normal = normal_data.xyz;
	float shininess = normal_data.w;
	float depth = texture(depth_texture, tex).x;

	vec3 pos = position_from_depth(tex, depth, inv_vp);
	vec3 viewdir = normalize(viewpos.xyz - pos);

	#if CUBEMAP == 1
	float glossiness = texture(albedo_texture, tex).w;
	vec3 cubemap_refl = texture(env_cubemap, reflect(-viewdir, normal)).rgb * glossiness * 2.0f; // Looks better with mul by 2.0f
	#else
	vec3 cubemap_refl = vec3(0.0f, 0.0f, 0.0f);
	#endif


	vec3 result = lit_blinn(light, pos, normal, viewdir, shininess, cubemap_refl);

	float shadow_value = 1.0f;
#if SHADOWMAP == 1
	// We try to compare current pixel depth from the position of a light source with the depth
	// we get from the shadowmap.
	// pos - position in the world-space
	// shadowmap_pos - projected position
	vec4 shadowmap_pos = light.lightvp * vec4(pos, 1.0f);

	vec2 shadowmap_coord_offset = vec2(0.0f, 0.0f);
	vec2 shadowmap_coord_scale = vec2(1.0f, 1.0f);
	#ifdef DIRECTIONAL_CSM
	int cascade = calculate_cascade(linearized_depth(depth, znear, zfar), light);
	float fcascade = cascade;
	shadowmap_pos.xyz *= light.csm_scale[cascade].xyz;
	shadowmap_pos.xyz += light.csm_offset[cascade].xyz;
	shadowmap_coord_offset.x += fcascade / light.cascades_number;
	shadowmap_coord_scale.x *= (1.0f / light.cascades_number);
	#endif

#ifndef CASCADED_SHADOWMAP_DEBUG
	// clip space - all parameters lie in [-1; 1] interval
	vec3 shadowmap_clip_pos = shadowmap_pos.xyz / shadowmap_pos.w;
	
	if (shadowmap_clip_pos.x < -0.999f || shadowmap_clip_pos.y < -0.999f ||
		shadowmap_clip_pos.x > 0.999f || shadowmap_clip_pos.y > 0.999f)
		shadow_value = 1.0f;
	else
	{
		float pixel_depth = shadowmap_clip_pos.z * 0.5f + 0.5f;
		shadow_value = get_shadow_value(shadowmap_texture, pixel_depth, (shadowmap_clip_pos.xy * 0.5f + 0.5f) * shadowmap_coord_scale + shadowmap_coord_offset, light.shadowmap_params.x);
	}
#else
	result = cascade_color[cascade];
#endif // CASCADED_SHADOWMAP_DEBUG
#endif // SHADOWMAP

	color = vec4(result * shadow_value, 0.0f);
}
