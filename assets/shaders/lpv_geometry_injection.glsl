struct VSOutput
{
    vec3 normal;
    float surfel_area;
};

[include "sh.h"]

[vertex]

[sampler2D normal_texture 0]
[sampler2D depth_texture 2]

[uniform InjectionSettings 0 permodel]
uniform InjectionSettings
{
    vec4 injection_settings;
    mat4 rsm_to_world;
    mat4 world_to_lpv;
    vec4 light_parameters;
};

out VSOutput vsoutput;

void main()
{
    int grid_size = int(injection_settings.x);
    float cell_size = injection_settings.y;
    float occlusion_coeff = injection_settings.z;

    ivec2 texsize = textureSize(normal_texture, 0);
    int index = gl_VertexID;
    int x = index % texsize.x;
    int y = index / texsize.x;
    vec2 texcoord = vec2(float(x) / float(texsize.x), float(y) / texsize.x);
    vec3 normal = textureLod(normal_texture, texcoord, 0.0f).xyz;
    float depth = textureLod(depth_texture, texcoord, 0.0f).x;

    vec3 position_ws = position_from_depth(texcoord, depth, rsm_to_world);

    vec3 position_lpv = (world_to_lpv * vec4(position_ws, 1.0f)).xyz; // xyz in the [0, 1] interval

    float surfel_area;
    if (dot(normal, normal) < 0.001f)
        surfel_area = 0.0f; // surfel doesn't contribute to the occlusion
    else
        surfel_area = 4.0f / (texsize.x * texsize.y); // parallel projection only

    vsoutput.normal = normal;
    vsoutput.surfel_area = surfel_area / (cell_size * cell_size) * occlusion_coeff;

    //position_lpv += normal * 0.5f / grid_size;
    position_lpv.xy = position_lpv.xy * 2.0f - 1.0f; // move to the clip space
    position_lpv.z *= grid_size;

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

layout (location = 0) out vec4 out_occlusion_sh;

void main()
{
    vec3 normal = gsoutput.normal;
    SH4 sh = sh_cosine_lobe_sh4(normal);
    out_occlusion_sh = sh.c * gsoutput.surfel_area;
}
