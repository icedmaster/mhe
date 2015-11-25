[sampler2D albedo_texture 0]
[sampler2D normal_texture 1]
[sampler2D accumulator_texture 2]
[sampler2D depth_texture 3]

struct GBuffer
{
	vec3 albedo;
	vec3 normal;
	vec3 baked_irradiance;
	float depth;
	float glossiness;
	float shininess;
};

GBuffer gbuffer_unpack(vec2 tex)
{
	GBuffer gbuffer;
	vec4 layer0 = texture(albedo_texture, tex);
	vec4 layer1 = texture(normal_texture, tex);
	vec4 layer2 = texture(accumulator_texture, tex);
	gbuffer.albedo = layer0.rgb;
	gbuffer.normal = layer1.rgb;
	gbuffer.baked_irradiance = layer2.rgb;
	gbuffer.glossiness = layer0.a;
	gbuffer.shininess = layer1.a;
	gbuffer.depth = texture(depth_texture, tex).r;

	return gbuffer;
}

float gbuffer_depth(vec2 tex)
{
	return texture(depth_texture, tex).r;
}

vec3 gbuffer_albedo(vec2 tex)
{
	return texture(albedo_texture, tex).rgb;
}

vec3 gbuffer_normal(vec2 tex)
{
	return texture(normal_texture, tex).xyz;
}

vec3 gbuffer_normal(vec2 tex, ivec2 offset)
{
	return textureOffset(normal_texture, tex, offset).xyz;
}

vec3 gbuffer_baked_light(vec2 tex)
{
	return texture(accumulator_texture, tex).rgb;
}

