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

[sampler2D albedo_texture 0]
[sampler2D light_texture 1]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	vec4 albedo = texture(albedo_texture, vsoutput.tex);
	vec4 light = texture(light_texture, vsoutput.tex);

	color = albedo + light;
}