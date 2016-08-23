[defs STAGE 0 1]

[compute]

[var THREADS 4]
[var REDUCTION_THREADS 16]

[include "../sh.h"]

#if STAGE == 0
// TODO: use HDR cubemaps
layout(rgba8, binding = 0) readonly uniform imageCube cubemap;

layout(std430, binding = 0) buffer OutputData
{
	ColorSH9 output_data[];
};

shared ColorSH9 tmp_color[THREADS * THREADS];

layout(local_size_x = THREADS, local_size_y = THREADS) in;
void main()
{
    vec3 directions_ws[6] = vec3[6](vec3(1, 0, 0), vec3(-1, 0, 0), vec3(0, 1, 0), vec3(0, -1, 0), vec3(0, 0, 1), vec3(0, 0, -1));
    ivec2 size = imageSize(cubemap);
    ColorSH9 result = sh9_zero();
    for (int f = 0; f < 6; ++f)
    {
        vec3 dir = directions_ws[f];
        for (int x = 0; x < min(THREADS, size.x); ++x)
        {
            for (int y = 0; y < min(THREADS, size.y); ++y)
            {
                int xpos = int(gl_GlobalInvocationID.x * THREADS + x);
                int ypos = int(gl_GlobalInvocationID.y * THREADS + y);
                float u = float(xpos) / float(size.x) * 2.0f - 1.0f;
                float v = float(ypos) / float(size.y) * 2.0f - 1.0f;
                float weight = 1.0f + u * u + v * v;
                weight = 4.0f / (sqrt(weight) * weight);
                vec3 color = imageLoad(cubemap, ivec3(xpos, ypos, f)).rgb;
                ColorSH9 projected_color = sh9_project(dir, color * weight);
                result = add(result, projected_color);
            }
        }
    }
    
    tmp_color[gl_LocalInvocationIndex] = result;
    barrier();
    memoryBarrierShared();

    if (gl_LocalInvocationIndex == 0)
    {
        ColorSH9 workgroup_result = sh9_zero();
        for (int i = 0; i < THREADS * THREADS; ++i)
            workgroup_result = add(workgroup_result, tmp_color[i]);
        output_data[gl_WorkGroupID.x * gl_WorkGroupSize.y + gl_WorkGroupID.y] = workgroup_result;
    }
}
#endif // STAGE == 0

#if STAGE == 1
layout(std430, binding = 0) buffer InputData
{
    ColorSH9 input_data[];
};

layout(std430, binding = 1) buffer OutputData
{
    ColorSH9 output_data;
};

layout(binding = 2) uniform IntegrationSettings
{
    vec4 settings;
};

shared ColorSH9 shared_result[REDUCTION_THREADS];

layout(local_size_x = REDUCTION_THREADS) in;
void main()
{
    int iterations = int(settings.x);
    ColorSH9 result = sh9_zero();
    for (int i = 0; i < iterations; ++i)
        result = add(result, input_data[gl_LocalInvocationIndex * iterations + i]);
    shared_result[gl_LocalInvocationIndex] = result;
    barrier();
    memoryBarrierShared();
    if (gl_LocalInvocationIndex == 0)
    {
        float total_weight = settings.y;
        ColorSH9 output_result = sh9_zero();
        for (int i = 0; i < REDUCTION_THREADS; ++i)
            output_result = add(output_result, shared_result[i]);
        output_data = mul(output_result, total_weight);
    }
}
#endif // STAGE == 1
