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
