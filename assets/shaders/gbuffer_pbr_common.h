struct GBuffer
{
    vec3 albedo;
    vec3 normal;
    vec3 indirect_lighting;
    float roughness;
    float metalness;
    float depth;
};

#ifdef GBUFFER_WRITE
layout (location = 0) out vec4 out_layer0;
layout (location = 1) out vec4 out_layer1;
layout (location = 2) out vec4 out_layer2;
#else
[sampler2D gbuffer_layer0_texture 0]
[sampler2D gbuffer_layer1_texture 1]
[sampler2D gbuffer_layer2_texture 2]
[sampler2D depth_texture 3]
#endif

#ifdef GBUFFER_WRITE
void gbuffer_pack(GBuffer gbuffer)
{
    out_layer0 = vec4(gbuffer.albedo, gbuffer.roughness);
    out_layer1 = vec4(gbuffer.normal, gbuffer.metalness);
    out_layer2 = vec4(gbuffer.indirect_lighting, 0.0f);
}
#else
GBuffer gbuffer_unpack(vec2 tex)
{
    vec4 layer0 = texture(gbuffer_layer0_texture, tex);
    vec4 layer1 = texture(gbuffer_layer1_texture, tex);
    vec4 layer2 = texture(gbuffer_layer2_texture, tex);
    float depth = texture(depth_texture, tex).x;

    GBuffer gbuffer;
    gbuffer.albedo = layer0.rgb;
    gbuffer.normal = layer1.xyz;
    gbuffer.indirect_lighting = layer2.rgb;
    gbuffer.roughness = layer0.w;
    gbuffer.metalness = layer1.w;
    gbuffer.depth = depth;

    return gbuffer;
}

vec3 gbuffer_normal_ws(vec2 tex)
{
    return texture(gbuffer_layer1_texture, tex).xyz;
}

float gbuffer_roughness(vec2 tex)
{
    return texture(gbuffer_layer0_texture, tex).w;
}

float gbuffer_depth(vec2 tex)
{
    return texture(depth_texture, tex).x;
}
#endif

