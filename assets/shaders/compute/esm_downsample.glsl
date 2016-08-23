[compute]

layout(r32f, binding = 0) readonly uniform image2D src;
layout(r32f, binding = 1) writeonly uniform image2D dst;

layout(binding = 2) uniform ESMDownsampleSettings
{
    vec4 settings;
};

[var THREADS_NUMBER 32]

layout(local_size_x = THREADS_NUMBER, local_size_y = THREADS_NUMBER) in;
void main()
{
    float exponent = settings.x;
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    float accum = 0.0f;
    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            accum += exp(imageLoad(src, pos * 4 + ivec2(x, y)).x * exponent);
        }
    }
	imageStore(dst, pos, vec4(accum / 16.0f));
}
