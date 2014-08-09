struct VSOutput
{
	vec2 tex;
};

[vertex]

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 tex;

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}

[fragment]

uniform sampler2D main_texture;

in VSOutput vsoutput;

out vec4 color;

void main()
{
	color = texture(main_texture, vsoutput.tex);
}