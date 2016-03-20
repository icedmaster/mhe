[defs OCCLUSION 0 1]

[uniform InjectionSettings 0 permodel]
uniform InjectionSettings
{
    vec4 injection_settings;
    mat4 rsm_to_world;
    mat4 world_to_lpv;
    vec4 light_parameters;
};

struct VSOutput
{
    ivec3 pos;
};

[vertex]

flat out VSOutput vsoutput;

void main()
{
    int cell_size = int(injection_settings.x);
    int index = gl_VertexID;
    int z = index / (cell_size * cell_size);
    int y = (index % (cell_size * cell_size)) / cell_size;
    int x = index % cell_size;
    vsoutput.pos = ivec3(x, y, z);

    gl_Position = vec4(vec2(vsoutput.pos.xy) / float(cell_size) * 2.0f - 1.0f, 0, 1);
}

[geometry]

flat in VSOutput vsoutput[];

flat out VSOutput gsoutput;

layout(points) in;
layout(points, max_vertices = 1) out;

void main()
{
    gl_Layer = vsoutput[0].pos.z;
    gsoutput = vsoutput[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}

[fragment]

[include "sh.h"]

[sampler3D sh_r_texture 0]
[sampler3D sh_g_texture 1]
[sampler3D sh_b_texture 2]

#if OCCLUSION == 1
[sampler3D sh_occlusion_texture 3]
#endif

layout (location = 0) out vec4 out_r;
layout (location = 1) out vec4 out_g;
layout (location = 2) out vec4 out_b;

flat in VSOutput gsoutput;

RGBSH4 fetch_data(ivec3 pos)
{
    RGBSH4 res;
    res.r = texelFetch(sh_r_texture, pos, 0);
    res.g = texelFetch(sh_g_texture, pos, 0);
    res.b = texelFetch(sh_b_texture, pos, 0);
    return res;
}

RGBSH4 propagate_dir(ivec3 cell_pos, ivec3 dir, float amp)
{
    RGBSH4 adjacent_cell_stored_radiance = fetch_data(cell_pos + dir);

    vec3 offset[6] = vec3[6](vec3(-1, 0, 0), vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 1, 0),
                             vec3(0, 0, -1), vec3(0, 0, 1));

    vec3 adjancent_cell_center = cell_pos + dir;
    vec3 irradiance = VEC3_ZERO;
    RGBSH4 res = empty_rgbsh4();
    for (int f = 0; f < 6; ++f)
    {
        vec3 face_pos = offset[f] * 0.5f;
        vec3 face_dir = dir - face_pos;
        float len = length(face_dir);
        face_dir /= len;
        // calculate the solid angle
        float solid_angle = 0.0f;
        if (len > 0.5f)
            solid_angle = len >= 1.5f ? 22.95668f / (4.0f * 180.0f) : 24.26083f / (4.0f * 180.0f);
        SH4 face_dir_sh = sh4(face_dir);

        // TODO: Ideally, we should take 4 occlusion samples from the vertexes of the grid
#if OCCLUSION == 1
        vec4 stored_occlusion_sh = texelFetch(sh_occlusion_texture, cell_pos + dir, 0);
        float occlusion = saturate(1.0f - dot(stored_occlusion_sh, face_dir_sh.c));
#else
        float occlusion = 1.0f;
#endif

        SH4 offset_cosine_lobe = sh_cosine_lobe_sh4(offset[f]);
        // calculate the amount of lighting in that direction
        vec3 radiance_at_dir = solid_angle * occlusion * max(VEC3_ZERO, shdot(face_dir_sh, adjacent_cell_stored_radiance));
        // and back to SH
        RGBSH4 radiance_at_dir_sh = mul(offset_cosine_lobe, radiance_at_dir);
        radiance_at_dir_sh.r = radiance_at_dir_sh.r * amp;
        radiance_at_dir_sh.g = radiance_at_dir_sh.g * amp;
        radiance_at_dir_sh.b = radiance_at_dir_sh.b * amp;
        res = add(res, radiance_at_dir_sh);
    }
    return res;
}

void main()
{
    float amp = injection_settings.w;
    RGBSH4 res = propagate_dir(gsoutput.pos, ivec3(-1, 0, 0), amp);
    res = add(res, propagate_dir(gsoutput.pos, ivec3(1, 0, 0), amp));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, -1, 0), amp));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, 1, 0), amp));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, 0, -1), amp));
    res = add(res, propagate_dir(gsoutput.pos, ivec3(0, 0, 1), amp));

    out_r = res.r;
    out_g = res.g;
    out_b = res.b;
}
