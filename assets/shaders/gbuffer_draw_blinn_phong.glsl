[defs LIGHT_TYPE 0 2]
[defs SHADOWMAP 0 1]
[defs SHADOWMAP_QUALITY 0 2]

#define LIGHT_DIRECTION_FROM_SOURCE

[include "common.h"]
[include "lighting_common.h"]

// Define layout before "geometry_common.h" will be included
// TODO: SPOT_LIGHT is temorary here
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

[sampler2D normal_texture 0]
[sampler2D depth_texture 1]
#if SHADOWMAP == 1
[sampler2D shadowmap_texture 5]
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

#define PCF_DIVIDER (1.0f / ((PCF_TAPS * 2 + 1) * (PCF_TAPS * 2 + 1)))

#pragma optionNV (unroll all)
float get_shadow_value(sampler2D tex, float pixel_depth, vec2 texcoord, float bias)
{
	float shadow_value = 0.0f;
	for (int x = -PCF_TAPS; x <= PCF_TAPS; ++x)
	{
		for (int y= -PCF_TAPS; y <= PCF_TAPS; ++y)
		{
			float shadowmap_depth = textureOffset(tex, texcoord, ivec2(x, y)).x;
			shadow_value += pixel_depth < shadowmap_depth + bias ? 1.0f : 0.0f;
		}
	}
		
	return saturate(shadow_value * PCF_DIVIDER);
}

#endif	// SHADOWMAP

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

	vec3 result = lit_blinn(light, pos, normal, viewdir, shininess);

	float shadow_value = 1.0f;
#if SHADOWMAP == 1
	// We try to compare current pixel depth from the position of a light source with the depth
	// we get from the shadowmap.
	// pos - position in the world-space
	// shadowmap_pos - projected position
	vec4 shadowmap_pos = light.lightvp * vec4(pos, 1.0f);
	// clip space - all parameters lie in [-1; 1] interval
	vec3 shadowmap_clip_pos = shadowmap_pos.xyz / shadowmap_pos.w;
	
	if (shadowmap_clip_pos.x < -0.999f || shadowmap_clip_pos.y < -0.999f ||
		shadowmap_clip_pos.x > 0.999f || shadowmap_clip_pos.y > 0.999f)
		shadow_value = 1.0f;
	else
	{
		float pixel_depth = shadowmap_clip_pos.z * 0.5f + 0.5f;
		shadow_value = get_shadow_value(shadowmap_texture, pixel_depth, shadowmap_clip_pos.xy * 0.5f + 0.5f, light.shadowmap_params.x);
	}
#endif

	color = vec4(result * shadow_value, 0.0f);
}
