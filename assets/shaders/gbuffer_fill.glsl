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
	vsoutput.nrm = (normal * vec4(nrm, 0.0f)).xyz;

	gl_Position = vp * model * vec4(pos, 1.0f);
}

[fragment]

[sampler2D diffuse_texture 0]

in VSOutput vsoutput;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 normal;

void main()
{
	color = vec4(texture(diffuse_texture, vsoutput.tex).rgb, 1.0f);
	normal = vec4(normalize(vsoutput.nrm), 1.0f);
}
