[defs BOUNCE 0 1]

#define NORMALMAP 0

[include "sh.h"]

struct VSOutput
{
	vec3 nrm;
	vec2 tex;

#if BOUNCE == 1
	ColorSH9 baked_radiance;
#endif
};

[vertex]

[include "geometry_common.h"]

[samplerBuffer gi_buffer 2]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;

	vec4 nrm_os = vec4(nrm, 0.0f);
	vec4 pos_os = vec4(pos, 1.0f);

	vsoutput.nrm = (normal * nrm_os).xyz;

#if BOUNCE == 1
	ColorSH9 baked_radiance = fetch_sh9_color(gi_buffer, gl_VertexID);
	vsoutput.baked_radiance = baked_radiance;
#endif

	gl_Position = vp * model * pos_os;
}

[fragment]

[include "material_common.h"]

[sampler2D diffuse_texture 0]

[uniform material 2 permodel]
uniform material
{
	PhongMaterial material_data;
};

in VSOutput vsoutput;

layout (location = 0) out vec4 color;

void main()
{
	vec3 albedo = texture(diffuse_texture, vsoutput.tex).rgb;
	vec3 nrm_ts = vec3(0, 0, 1);
#if BOUNCE == 1
	vec3 baked_irradiance = calculate_irradiance(nrm_ts, vsoutput.baked_radiance);
#else
	vec3 baked_irradiance = vec3(1, 1, 1);
#endif
	color = vec4(gl_FrontFacing ? albedo * baked_irradiance : VEC3_ZERO, 1);
	//color = vec4(albedo * baked_irradiance, 1);
}
