struct VSOutput
{
	vec3 tex;
};

[vertex]

uniform transform
{
	mat4 inv_vp;
};

layout (location = 0) in vec4 pos;

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = (inv_vp * pos).xyz;
	gl_Position = pos;
}

[fragment]

[samplerCube cubemap 0]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	color = vec4(texture(cubemap, vsoutput.tex).xyz, 1.0f);
}