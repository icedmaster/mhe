struct VSOutput
{
    vec3 normal;
    vec3 flux;
    float layer;
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
    vec4 light_parameters;
};

out VSOutput vsoutput;

void main()
{
    int cell_size = int(injection_settings.x);

    ivec2 texsize = textureSize(normal_texture, 0);
    int index = gl_VertexID;
    int x = index % texsize.x;
    int y = index / texsize.x;
    vec2 texcoord = vec2(float(x) / float(texsize.x), float(y) / texsize.x);
    vec3 normal = textureLod(normal_texture, texcoord, 0.0f).xyz; // already normalized
    vec3 flux = textureLod(flux_texture, texcoord, 0.0f).rgb;
    float depth = textureLod(depth_texture, texcoord, 0.0f).x;

    vec3 position_ws = position_from_depth(texcoord, depth, rsm_to_world);

    // just move "inside" the LPV volume and divide by the number of cells
    vec3 position_lpv = (world_to_lpv * vec4(position_ws, 1.0f)).xyz; // xyz in the [0, 1] interval

    if (dot(normal, normal) < 0.001f)
        flux = vec3(0.0f, 0.0f, 0.0f); // so, this VPL doesn't contribute to the final lighting

    // FIXME: Should I move this to the RSM generation?
    float diffuse_term = max(dot(light_parameters.xyz, normal), 0.0f);

    position_lpv += normal * 0.5f / cell_size;

    vsoutput.normal = normal;
    vsoutput.flux = flux * diffuse_term;
    vsoutput.layer = position_lpv.z * cell_size;

    position_lpv = position_lpv * 2.0f - 1.0f;

    gl_Position = vec4(position_lpv, 1.0f);
}

[geometry]

in VSOutput vsoutput[];

out VSOutput gsoutput;

layout(points) in;
layout(points, max_vertices = 1) out;

bool is_outside(vec3 pos)
{
    return pos.x > 1.0f || pos.y > 1.0f || pos.z > 32.0f || pos.x < -1.0f || pos.y < -1.0f || pos.z < 0.0f;
}

void main()
{
    gsoutput = vsoutput[0];
    gl_Layer = int(vsoutput[0].layer);
    gl_Position = gl_in[0].gl_Position;
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
