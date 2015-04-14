#define SPOT_LIGHT 0
#define POINT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

#ifndef LIGHT_TYPE
#define LIGHT_TYPE POINT_LIGHT
#endif

struct Light
{
	vec4 diffuse; // w - angle for SPOT light, radius for OMNI
	vec4 specular; // w - attenuation a
	vec4 position; // w - attenuation b
	vec4 direction; // w - angle attenuation for SPOT
	mat4 lightvp;
    mat4 lightw;
	vec4 shadowmap_params;	// x - shadowmap bias
};

#define SPOT_LIGHT_ANGLE(light) light.diffuse.w
#define OMNI_LIGHT_RADIUS(light) light.diffuse.w
#define LIGHT_ATTENUATION(light) light.specular.w
#define LIGHT_ATTENUATION_SQ(light) light.position.w

vec3 lambert(vec3 diffuse, vec3 material_diffuse, vec3 lightdir, vec3 normal, float attenuation)
{
	float ndotl = dot(lightdir, normal);
	return diffuse * material_diffuse * saturate(ndotl) * attenuation;
}

vec3 blinn(vec3 specular, vec3 material_specular, vec3 lightdir, vec3 normal, vec3 viewdir, float shininess, float attenuation)
{
	float ndotl = dot(lightdir, normal);
	float enable = ndotl > 0.0f ? 1.0f : 0.0f;

	// The standart reflect() method requires incident vector. We have lightdir vector directed FROM the surface,
	// therefore it's easier to calculate reflected vector manually, besides we've calculated NdotL already
	vec3 reflected = -lightdir + 2.0f * ndotl * normal;
	float vdotr = saturate(dot(viewdir, reflected));

	return specular * material_specular * pow(vdotr, shininess) * enable * attenuation;
}

vec3 lit_blinn(Light light, vec3 pos, vec3 normal, vec3 viewdir, float shininess)
{
#if LIGHT_TYPE != POINT_LIGHT
	#ifdef LIGHT_DIRECTION_FROM_SOURCE
	vec3 light_direction = -light.direction.xyz;
	#else
	vec3 light_direction = light.direction.xyz;
	#endif
#endif

#if LIGHT_TYPE == DIRECTIONAL_LIGHT
	float attenuation = 1.0f;
	vec3 lightdir = light_direction;
#else
	vec3 lightray = light.position.xyz - pos;
	vec3 lightdir = normalize(lightray);
	float raylength = length(lightray);
#endif

#if LIGHT_TYPE == SPOT_LIGHT
	float attenuation = 1.0f / (1.0f + LIGHT_ATTENUATION(light) * raylength + LIGHT_ATTENUATION_SQ(light) * raylength * raylength);
	float angle = light.diffuse.w;
	float falloff = saturate(dot(lightdir, light_direction));
	float angle_attenuation = pow(falloff, light.direction.w);
	attenuation = falloff > angle ? attenuation * angle_attenuation : 0.0f;
#endif

#if LIGHT_TYPE == POINT_LIGHT
	float radius = light.diffuse.w;
	float attenuation = saturate(1.0f - raylength * raylength / (radius * radius));
	attenuation = raylength <= radius ? attenuation : 0.0f;
#endif

	return lambert(light.diffuse.rgb, vec3(1.0f), lightdir, normal, attenuation) + 
		blinn(light.specular.rgb, vec3(1.0f), lightdir, normal, viewdir, shininess, attenuation);
}
