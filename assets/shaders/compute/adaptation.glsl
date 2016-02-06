[compute]

layout(std430, binding = 0) buffer InputData
{
    float input_data;
};

layout(std430, binding = 1) buffer OutputData
{
	float output_data;
};

layout(binding = 2) uniform AdaptationSettings
{
    vec4 settings;
};

layout(local_size_x = 1) in;
void main()
{
    float rate = settings.x;
    float delta = settings.y;
    float current = settings.z;
    float value = input_data;
    output_data = current + (value - current) * delta / rate; // * (1 - exp(-delta * rate));
}
