[defs NORMALMAP 0 1]

struct VSOutput
{
	vec3 nrm;
	vec2 tex;
#if NORMALMAP == 1
	vec3 tng;
	vec3 bitng;
#endif
};

[vertex]

[include "geometry_common.h"]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	vsoutput.nrm = (normal * vec4(nrm, 0.0f)).xyz;
#if NORMALMAP == 1
	vsoutput.tng = tng;
	// TODO: add handedness
	vsoutput.bitng = cross(nrm, tng);
#endif

	gl_Position = vp * model * vec4(pos, 1.0f);
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
	color = vec4(texture(diffuse_texture, vsoutput.tex).rgb, 1.0f);
#if NORMALMAP == 0
	normal = vec4(normalize(vsoutput.nrm), PHONG_MATERIAL_SHININESS(material_data));
#else
	vec3 normal_tngspace = texture(normalmap_texture, vsoutput.tex).xyz * 2.0f - 1.0f;
	vec3 normal_wspace = vsoutput.tng * normal_tngspace.x + vsoutput.bitng * normal_tngspace.y + vsoutput.nrm * normal_tngspace.z;
	normal = vec4(normal_wspace, PHONG_MATERIAL_SHININESS(material_data));
#endif
}
