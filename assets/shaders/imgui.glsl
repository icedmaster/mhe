struct VSOutput
{
	vec2 tex;
	vec4 col;
};

[vertex]

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 tex;

[uniform transform 0 perframe]
uniform transform
{
	mat4 vp;
};

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	vsoutput.col = col;

	gl_Position = vp * pos;
}

[fragment]

in VSOutput vsoutput;

out vec4 color;

[sampler2D font_texture 0]

void main()
{
	color = texture(font_texture, vsoutput.tex) * vsoutput.col;
}
