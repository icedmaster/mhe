#define LUMINANCE_VECTOR vec3(0.299, 0.587, 0.114)

float calculate_luminance(vec3 c)
{
    return dot(c, LUMINANCE_VECTOR);
}

vec3 exposed_color(vec3 color, float key, float average_luminance, float threshold_luminance)
{
    float exposure = max(key / max(average_luminance, 0.001f), 0.001f);
    float logariphmic_exposure = log2(exposure) - threshold_luminance;
    return exp2(logariphmic_exposure) * color;
}

