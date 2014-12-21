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

#define AMBIENT_COEFF 0.1f

void main()
{
	vec4 albedo = texture(albedo_texture, vsoutput.tex);
	vec4 light = texture(light_texture, vsoutput.tex);

	float shadow = light.w + AMBIENT_COEFF;
	// TODO: this is a hack - need to fix it with ambient lighting
	float light_coeff = shadow > 0.999f ? 1.0f : 0.0f; 
	color = vec4(albedo.rgb * shadow, albedo.a) + vec4(light.rgb * light_coeff, 0.0f);
}