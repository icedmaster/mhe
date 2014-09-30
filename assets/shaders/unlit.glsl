struct VSOutput
{
	vec2 tex;
};

[vertex]

uniform transform
{
	mat4 vp;
};

uniform permodel
{
	mat4 model;
};

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec3 tng;
layout (location = 3) in vec2 tex;

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;

	gl_Position = vp * model * vec4(pos, 1.0f);
}

[fragment]

[sampler2D texture 0]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	color = texture2D(texture, vsoutput.tex);
}
