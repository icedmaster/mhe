struct VSOutput
{
	vec3 nrm;
	vec2 tex;
};

[vertex]

[include "geometry_common.h"]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;

	gl_Position = vp * model * vec4(pos, 1.0f);
}

[fragment]

[include "material_common.h"]

[sampler2D diffuse_texture 0]

in VSOutput vsoutput;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(diffuse_texture, vsoutput.tex);
}
