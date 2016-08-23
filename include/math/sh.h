#ifndef __SH_H__
#define __SH_H__

#include "math_utils.hpp"
#include "core/array.hpp"

namespace mhe {

template <class T, size_t B>
struct SH
{
    static const size_t bands = B;
    static const size_t coefficients_number = B * B;
    typedef T datatype;

    mhe::array<T, coefficients_number> coeff;

    SH()
    {
        coeff.fill(T());
    }

    void set(T theta, T phi)
    {
        for (int l = 0; l < bands; ++l)
        {
            for (int m = -l; m <= l; ++m)
            {
                coeff[sh_index(l, m)] = cals_sh(l, m, theta, phi);
            }
        }
    }

    template <class NT, size_t NB>
    SH<T, B>& operator+= (const SH<NT, NB>& h)
    {
        for (size_t i = 0; i < coefficients_number; ++i)
            coeff[i] += h.coeff[i];
        return *this;
    }
};

template <class T, size_t B, class U>
SH<T, B> operator* (const SH<T, B>& h, U c)
{
    SH<T, B> res;
    for (size_t i = 0; i < h.coeff.size(); ++i)
        res.coeff[i] = h.coeff[i] * c;
    return res;
}

inline float sh9_calculate_cubemap_projection_weight(size_t width, size_t height)
{
    float weight = 0.0f;
    for (size_t x = 0; x < width; ++x)
    {
        for (size_t y = 0; y < height; ++y)
        {
            float u = static_cast<float>( x ) / width * 2.0f - 1.0f;
            float v = static_cast<float>( y ) / height * 2.0f - 1.0f;
            float w = 1.0f + u * u + v * v;
            w = 4.0f / (sqrt(w) * w);
            weight += w;
        }
    }
    
    return 6 * pi / weight;
}

}

#endif
