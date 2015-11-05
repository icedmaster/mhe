[defs NORMALMAP 0 1]

struct VSOutput
{
	vec3 nrm;
	vec2 tex;
#if NORMALMAP == 1
	vec3 tng;
	vec3 bitng;
#endif
	vec3 baked_diffuse;
};

[vertex]

[include "sh.h"]
[include "geometry_common.h"]

[samplerBuffer gi_buffer 2]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;

	vec4 nrm_os = vec4(nrm, 0.0f);
	vec4 pos_os = vec4(pos, 1.0f);
	vec4 tng_os = vec4(tng.xyz, 0.0f);

	vsoutput.nrm = (normal * nrm_os).xyz;
#if NORMALMAP == 1
	vsoutput.tng = (normal * tng_os).xyz;
	vsoutput.bitng = cross(vsoutput.nrm, vsoutput.tng) * tng.w;
#endif

	vec3 light = vec3(0, 1, 0) * saturate(dot(vsoutput.nrm, vec3(0, 1, 0)));
	ColorSH9 baked_radiance = fetch_sh9_color(gi_buffer, gl_VertexID);
	vec3 baked_irradiance = calculate_irradiance(vsoutput.nrm, baked_radiance);
	vsoutput.baked_diffuse = light + baked_irradiance;

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

void main()
{
	color = vec4(vsoutput.baked_diffuse, 1);
}
