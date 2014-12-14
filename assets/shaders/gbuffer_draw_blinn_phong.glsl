#define MAX_LIGHTS_NUMBER 8

[defs LIGHT_TYPE 0 2]
[defs LIGHTS_NUMBER 1 4]
[defs SHADOWMAP 0 1]
[defs SHADOWMAP_LIGHT 0 3]

#define LIGHT_DIRECTION_FROM_SOURCE

[include "common.h"]
[include "lighting_common.h"]

uniform transform
{
	mat4 vp;
	mat4 inv_proj;
	mat4 inv_vp;
	vec4 viewpos;
};

struct VSOutput
{
	vec2 tex;
};

[vertex]

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 tex;

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}

[fragment]

[sampler2D normal_texture 0]
[sampler2D depth_texture 1]
#if SHADOWMAP == 1
[sampler2D shadowmap_texture 5]
#endif

uniform lights
{
	Light light[MAX_LIGHTS_NUMBER];
};

in VSOutput vsoutput;

out vec4 color;

void main()
{
	vec3 normal = texture(normal_texture, vsoutput.tex).xyz;
	float depth = texture(depth_texture, vsoutput.tex).x;

	vec3 pos = position_from_depth(vsoutput.tex, depth, inv_vp);
	vec3 viewdir = normalize(viewpos.xyz - pos);

	vec3 result = vec3(0.0f);
	for (int i = 0; i < LIGHTS_NUMBER; ++i)
	{
		result.rgb += lit_blinn(light[i], pos, normal, viewdir);
	}

	float shadow_value = 1.0f;
#if SHADOWMAP == 1
	// We try to compare current pixel depth from the position of a light source with the depth
	// we get from the shadowmap.
	// pos - position in the world-space
	// shadowmap_pos - projected position
	vec4 shadowmap_pos = light[SHADOWMAP_LIGHT].lightvp * vec4(pos, 1.0f);
	// clip space - all parameters lie in [-1; 1] interval
	vec3 shadowmap_clip_pos = shadowmap_pos.xyz / shadowmap_pos.w;
	float pixel_depth = shadowmap_clip_pos.z;
	// move texcoord to the [0; 1] interval
	float shadowmap_depth = texture(shadowmap_texture, shadowmap_clip_pos.xy * 0.5f + 0.5f).x;
	// TODO: add PCF
	shadow_value = pixel_depth < shadowmap_depth + light[SHADOWMAP_LIGHT].shadowmap_params.x ? 1.0f : 0.1f;
#endif

	color = vec4(result, shadow_value);
}