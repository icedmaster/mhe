[compute]

layout(r32f, binding = 0) readonly uniform image2D src;

layout(std430, binding = 1) buffer OutputData
{
	float output_data[];
};

[var THREADS_NUMBER 32]

shared float workgroup_data[THREADS_NUMBER];

layout(local_size_x = THREADS_NUMBER, local_size_y = THREADS_NUMBER) in;
void main()
{
    ivec2 src_pos = ivec2(gl_GlobalInvocationID.xy);
    float f = imageLoad(src, src_pos).x;
    uint dst_pos = gl_GlobalInvocationID.y * gl_WorkGroupSize.x * gl_NumWorkGroups.x + gl_GlobalInvocationID.x;
    output_data[dst_pos] = f;
}
