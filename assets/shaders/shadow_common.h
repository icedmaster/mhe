#if SHADOWMAP == 1

layout (binding = 5) uniform sampler2D shadowmap_texture;

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
[include "lighting_common.h"]

#pragma optionNV (unroll all)

float pcf_filter(sampler2D tex, float pixel_depth, vec2 texcoord, float bias, vec2 sample_size)
{
	float shadow_value = 0.0f;
	float weight = 0.0f;
	vec2 f = fract(texcoord / sample_size);
	float left_edge = 1.0f - f.x;
	float right_edge = f.x;
	float top_edge = f.y;
	float bottom_edge = 1.0 - f.y;
	for (int y = -PCF_TAPS; y <= PCF_TAPS; ++y)
	{
		for (int x = -PCF_TAPS; x <= PCF_TAPS; ++x)
		{
			vec2 offset = vec2(x * sample_size.x, y * sample_size.y);
			float shadowmap_depth = texture(tex, texcoord + offset).x;
			float tap_weight = pcf_weights[(y + PCF_TAPS) * PCF_SIZE + x + PCF_TAPS];
			if (x == -PCF_TAPS) tap_weight = left_edge;
			else if (x == PCF_TAPS) tap_weight = right_edge;
			if (y == -PCF_TAPS) tap_weight *= bottom_edge;
			else if (y == PCF_TAPS) tap_weight *= top_edge;
			shadow_value += pixel_depth < shadowmap_depth + bias ? 1.0f * tap_weight : 0.0f;
			weight += 1.0f;
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

#ifdef DIRECTIONAL_CSM
int calculate_cascade_scsm(vec3 pos_ws, Light light)
{
    for (int i = 0; i < light.cascades_number; ++i)
    {
        vec3 dir = pos_ws - light.scsm_params[i].xyz;
        float dist_sq = dot(dir, dir);
        if (dist_sq < light.scsm_params[i].w)
            return i;
    }
    return light.cascades_number;
}
#endif

float get_shadow_value(sampler2D tex, float pixel_depth, vec2 texcoord, float bias, vec2 sample_size)
{
#if SHADOWMAP_QUALITY != 0
	#ifndef SHADOWMAP_DISC_FILTER
	return pcf_filter(tex, pixel_depth, texcoord, bias, sample_size);
	#else
	return disc_filter(tex, pixel_depth, texcoord, bias);
	#endif
#else
	return pixel_depth < texture(tex, texcoord).r + bias ? 1.0f : 0.0f;
#endif
}
#endif

float get_shadow_value(vec3 pos_01, vec3 pos_ws, float linear_depth, Light light)
{
#if SHADOWMAP == 0
    return 1.0f;
#else
	vec2 sample_size = 1.0f / textureSize(shadowmap_texture, 0);
#ifndef DIRECTIONAL_CSM
	vec4 shadowmap_pos_cs = light.lightvp * vec4(pos_ws, 1.0f);
    shadowmap_pos_cs.xyz = shadowmap_pos_cs.xyz / shadowmap_pos_cs.w;
#else
	vec2 shadowmap_coord_offset = vec2(0.0f, 0.0f);
	vec2 shadowmap_coord_scale = vec2(1.0f, 1.0f);
	int cascade = calculate_cascade_scsm(pos_ws, light);
	if (cascade == light.cascades_number) return 1.0f;
	float fcascade = cascade;
	float bias_scale = fcascade + 1.0f;
	vec4 shadowmap_pos_cs = light.lightvp[cascade] * vec4(pos_ws, 1.0f);
	shadowmap_pos_cs.xyz *= light.csm_scale[cascade].xyz;
	shadowmap_pos_cs.xyz += light.csm_offset[cascade].xyz;
	shadowmap_coord_offset.x += fcascade / light.cascades_number;
	shadowmap_coord_scale.x *= (1.0f / light.cascades_number);

	sample_size *= (1.0f / (fcascade + 1.0f));
#endif

	if (shadowmap_pos_cs.x < -0.999f || shadowmap_pos_cs.y < -0.999f ||
		shadowmap_pos_cs.x > 0.999f || shadowmap_pos_cs.y > 0.999f)
		return 1.0f;
	else
	{
        vec3 shadowmap_pos_01 = shadowmap_pos_cs.xyz * 0.5f + 0.5f;
#ifndef DIRECTIONAL_CSM
        return get_shadow_value(shadowmap_texture, shadowmap_pos_01.z, shadowmap_pos_01.xy, light_shadowmap_bias(light), sample_size);
#else
		return get_shadow_value(shadowmap_texture, shadowmap_pos_01.z, shadowmap_pos_01.xy * shadowmap_coord_scale + shadowmap_coord_offset,
                                light_shadowmap_bias(light), sample_size);
#endif
	}
#endif // SHADOWMAP
}


