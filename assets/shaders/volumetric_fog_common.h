layout(binding = 1) uniform Settings
{
    vec4 volume_size;
    vec4 fog_color;
    vec4 fog_settings;
};

float get_fog_range()
{
    return volume_size.w;
}
