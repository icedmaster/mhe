[defs NORMALMAP 0 1]
[defs SKINNING 0 1]

#define CUSTOM_PERCAMERA_UNIFORM
[include "geometry_common.h"]

struct VSOutput
{
	vec3 nrm;
	vec2 tex;
#if NORMALMAP == 1
	vec3 tng;
	vec3 bitng;
#endif
};

[uniform ShaderData 0 percamera]
uniform ShaderData
{
    mat4 vp;
    vec4 settings;
    vec4 light_diffuse_color;
};

[vertex]

[include "skinning_common.h"]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;

#if SKINNING == 0
	vec4 nrm_os = vec4(nrm, 0.0f);
	vec4 pos_os = vec4(pos, 1.0f);
	vec4 tng_os = vec4(tng.xyz, 0.0f);
#else
	mat4 skinning_transform;
	skinning(skinning_transform, skin_texture, ids, weights);
	vec4 nrm_os = skinning_transform * vec4(nrm, 0.0f);
	vec4 pos_os = skinning_transform * vec4(pos, 1.0f);
	vec4 tng_os = skinning_transform * vec4(tng.xyz, 0.0f);
#endif

	vsoutput.nrm = (normal * nrm_os).xyz;
#if NORMALMAP == 1
	vsoutput.tng = (normal * tng_os).xyz;
	vsoutput.bitng = cross(vsoutput.nrm, vsoutput.tng) * tng.w;
#endif

	gl_Position = vp * model * pos_os;
}

[fragment]

[include "material_common.h"]
[include "common.h"]

[sampler2D diffuse_texture 0]
#if NORMALMAP == 1
[sampler2D normalmap_texture 2]
#endif

[uniform material 2 permodel]
uniform material
{
	PhongMaterial material_data;
};

in VSOutput vsoutput;

layout (location = 0) out vec3 out_normal;
layout (location = 1) out vec3 out_accumulator;

void main()
{
	vec3 color = texture(diffuse_texture, vsoutput.tex).rgb;
#if NORMALMAP == 0
	vec3 normal_wspace = normalize(vsoutput.nrm);
#else
	vec3 normal_tngspace = texture(normalmap_texture, vsoutput.tex).xyz * 2.0f - 1.0f;
	vec3 normal_wspace = vsoutput.tng * normal_tngspace.x + vsoutput.bitng * normal_tngspace.y + vsoutput.nrm * normal_tngspace.z;
    normal_wspace = normalize(normal_wspace);
#endif

    float flux_reflectance_coeff = settings.x;
    float reflected_flux = flux_reflectance_coeff * PHONG_MATERIAL_GLOSSINESS(material_data);

    // output values
    out_normal = normal_wspace;
    out_accumulator = color * light_diffuse_color.rgb * reflected_flux / PI;
}
