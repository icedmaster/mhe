struct VSOutput
{
    vec3 normal;
    vec3 flux;
};

[include "sh.h"]

[vertex]

[sampler2D normal_texture 0]
[sampler2D flux_texture 1]
[sampler2D depth_texture 2]

[uniform InjectionSettings 0 permodel]
uniform InjectionSettings
{
    vec4 injection_settings;
    mat4 rsm_to_world;
    mat4 world_to_lpv;
};

out VSOutput vsoutput;

void main()
{
    int cell_size = int(injection_settings.x);

    ivec2 texsize = textureSize(normal_texture, 0);
    int index = gl_VertexID;
    vec2 texcoord = vec2(float(index % texsize.x) / float(texsize.x), float(index / texsize.x) / texsize.x);
    vec3 normal = textureLod(normal_texture, texcoord, 0.0f).xyz;
    vec3 flux = textureLod(flux_texture, texcoord, 0.0f).rgb;
    float depth = textureLod(depth_texture, texcoord, 0.0f).x;

    vec3 position_ws = position_from_depth(texcoord, depth, rsm_to_world);

    vec3 position_lpv = (world_to_lpv * vec4(position_ws, 1.0f)).xyz;

    if (dot(normal, normal) < 0.001f)
        flux = vec3(0.0f, 0.0f, 0.0f);

    position_lpv += normal * 0.5f / cell_size;
    position_lpv.xy = position_lpv.xy * 2.0f - 1.0f;
    position_lpv.z *= cell_size;

    vsoutput.normal = normal;
    vsoutput.flux = flux;

    gl_Position = vec4(position_lpv, 1.0f);
}

[geometry]

in VSOutput vsoutput[];

out VSOutput gsoutput;

layout(points) in;
layout(points, max_vertices = 1) out;

void main()
{
    gsoutput = vsoutput[0];
    gl_Layer = int(gl_in[0].gl_Position.z);
    gl_Position = gl_in[0].gl_Position;
    gl_Position.z = 0.5f;
    EmitVertex();
    EndPrimitive();
}

[fragment]

in VSOutput gsoutput;

layout (location = 0) out vec4 out_r;
layout (location = 1) out vec4 out_g;
layout (location = 2) out vec4 out_b;

void main()
{
    vec3 normal = gsoutput.normal;
    SH4 sh = sh_cosine_lobe_sh4(normal);
    
    RGBSH4 color = mul(sh, gsoutput.flux);
    out_r = color.rgb[0];
    out_g = color.rgb[1];
    out_b = color.rgb[2];
}
