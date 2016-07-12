[include "common.h"]

float brdf_lambert(float ndotl)
{
    return ndotl;
}

float smith_ggx_visibility(float ndotl, float ndotv, float roughness)
{
    float a = roughness * roughness;
    float lambda_l = ndotv * sqrt((-ndotl * a + ndotl) * ndotl + a);
    float lambda_v = ndotl * sqrt((-ndotv * a + ndotv) * ndotv + a);
    return 0.5f / (lambda_l + lambda_v);
}

float ggx_ndf(float ndoth, float roughness)
{
    float m = roughness * roughness;
    float f = (ndoth * m - ndoth) * ndoth + 1;
    return m / (f * f);
}

float fresnel_schlick(float f0, float f90, float ldoth)
{
    return f0 + (f90 - f0) * pow(1.0f - ldoth, 5.0f);
}

vec3 BRDF_lambert_ggx(GBuffer gbuffer, vec3 diffuse, vec3 specular, float ndotl, float ndotv, float ndoth, float ldoth)
{
    float fd = brdf_lambert(ndotl) / PI;
    // specular term
    float V = smith_ggx_visibility(ndotl, ndotv, gbuffer.roughness);
    float D = ggx_ndf(ndoth, gbuffer.roughness);
    
    float f0 = 0.0f;
    float f90 = 0.5f + 2.0f * ldoth * ldoth * gbuffer.roughness;
    float F = fresnel_schlick(f0, f90, ldoth);

    float fs = V * D * F / PI;

    return fd * diffuse + fs * specular;
}
