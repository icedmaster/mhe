#define MAX_LIGHTS_NUMBER 4

[defs LIGHT_TYPE 0 2]
[defs LIGHTS_NUMBER 1 4]

#define LIGHT_DIRECTION_FROM_SOURCE

[include "common.h"]
[include "lighting_common.h"]

#ifndef LIGHT_TYPE
#define LIGHT_TYPE SPOT_LIGHT
#endif

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
/*
#if LIGHT_TYPE == DIRECTIONAL_LIGHT
		vec3 lightdir = normalize(-light[i].direction.xyz);
		float attenuation = 1.0f;
#else
		vec3 lightray = light[i].position.xyz - pos;
		vec3 lightdir = normalize(lightray);
		float raylength = length(lightray);
		float attenuation_coeff = light[i].position.w;
		float attenuation = 1.0f / ( attenuation_coeff * attenuation_coeff * raylength );
#endif

		float ndotl = dot(lightdir, normal);
#if LIGHT_TYPE == SPOT_LIGHT
		float angle = light[i].diffuse.w;
		float falloff = saturate(dot(lightdir, -light[i].direction.xyz));
		float angle_attenuation = pow(falloff, light[i].direction.w);
		attenuation = falloff > angle ? attenuation * angle_attenuation : 0.0f;
#endif

#if LIGHT_TYPE == POINT_LIGHT
		float radius = light[i].diffuse.w;
		attenuation = raylength <= radius ? attenuation : 0.0f;
#endif

		result.rgb += light[i].diffuse.rgb * saturate(ndotl) * attenuation;
		// specular
		float specular = ndotl >= 0.0f ? pow(saturate(dot(mhe_reflect(-lightdir, normal), viewdir)), 50.0f) : 0.0f;
		result.rgb += light[i].specular.rgb * specular * attenuation;
*/
		result.rgb += lit_blinn(light[i], pos, normal, viewdir);
	}

	color = vec4(result, 0.0f);
}