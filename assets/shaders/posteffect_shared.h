layout(std430, binding = 0) buffer LuminanceBuffer
{
    float average_luminance;
};

[uniform BloomSettings 1 permodel]
uniform BloomSettings
{
    vec4 settings;
};

void unpack_bloom_settings(out float key, out float threshold, out float intensity)
{
    key = settings.x;
    threshold = settings.y;
    intensity = settings.z;
}
