[defs NORMALMAP 0 1]
[defs SKINNING 0 1]
[defs BAKED_LIGHT 0 1]

[include "sh.h"]
[include "geometry_common.h"]

struct VSOutput
{
	vec3 nrm;
	vec2 tex;
#if NORMALMAP == 1
	vec3 tng;
	vec3 bitng;
#endif
#if BAKED_LIGHT == 1
	ColorSH9 baked_radiance;
#endif
};

[vertex]

[include "skinning_common.h"]

#if BAKED_LIGHT == 1
[samplerBuffer gi_buffer 2]
#endif

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

#if BAKED_LIGHT == 1
    vsoutput.baked_radiance = fetch_sh9_color(gi_buffer, gl_VertexID);
#endif

	gl_Position = vp * model * pos_os;
}

[fragment]

[include "material_common.h"]

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

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 accumulator;

void main()
{
	color = vec4(texture(diffuse_texture, vsoutput.tex).rgb, PHONG_MATERIAL_GLOSSINESS(material_data));
#if NORMALMAP == 0
	normal = vec4(normalize(vsoutput.nrm), PHONG_MATERIAL_SHININESS(material_data));
	vec3 normal_tngspace = vec3(0, 0, 1);
#else
	vec3 normal_tngspace = texture(normalmap_texture, vsoutput.tex).xyz * 2.0f - 1.0f;
	vec3 normal_wspace = vsoutput.tng * normal_tngspace.x + vsoutput.bitng * normal_tngspace.y + vsoutput.nrm * normal_tngspace.z;
	normal = vec4(normalize(normal_wspace), PHONG_MATERIAL_SHININESS(material_data));
#endif

#if BAKED_LIGHT == 1
	vec3 baked_irradiance = calculate_irradiance(normal_tngspace, vsoutput.baked_radiance);
    accumulator = vec4(baked_irradiance, 1);
#else
	accumulator = vec4(ambient.rgb, 1);
#endif
}
