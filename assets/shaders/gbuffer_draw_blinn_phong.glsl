#define MAX_LIGHTS_NUMBER 4

[defs LIGHT_TYPE 0 2]
[defs LIGHTS_NUMBER 1 4]

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

	color = vec4(result, 0.0f);
}