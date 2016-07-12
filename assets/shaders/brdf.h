[include "common.h"]

float brdf_lambert(float ndotl)
{
    return ndotl;
}

float smith_ggx_visibility(float ndotl, float ndotv, float roughness)
{
    float a = roughness * roughness * 0.5f;
    float ll = ndotl * (1.0f - a) + a;
    float lv = ndotv * (1.0f - a) + a;
    return 1.0f / (ll * lv + 0.0001f);
}

float ggx_ndf(float ndoth, float roughness)
{
    float m = roughness * roughness;
    float f = (ndoth * m - ndoth) * ndoth + 1.0f;
    return m / (f * f);
}

vec3 fresnel_schlick(vec3 f0, float ldoth)
{
    return f0 + (1.0f - f0) * pow(1.0f - ldoth, 5.0f);
}

vec3 BRDF_lambert_ggx(GBuffer gbuffer, vec3 diffuse, vec3 specular, float ndotl, float ndotv, float ndoth, float ldoth)
{
    float fd = brdf_lambert(ndotl) / PI;
    // specular term
    float V = smith_ggx_visibility(ndotl, ndotv, gbuffer.roughness);
    float D = ggx_ndf(ndoth, gbuffer.roughness);
    
    vec3 F = fresnel_schlick(specular, ldoth);

    float fs = max(V * D / PI, 0.0f);

    return mix(fd * diffuse, fs * F, gbuffer.metalness) * ndotl;
}
