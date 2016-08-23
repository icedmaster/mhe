#ifndef __SH_H__
#define __SH_H__

#include "math_utils.hpp"
#include "core/array.hpp"

namespace mhe {

namespace sh {
inline int double_factorial(int n)
{
    if (n <= 1) return 1;
    return n * double_factorial(n - 2);
}

template <class T>
T legendre(int l, int m, T x)
{
    // I do not include Condon-Shortley phase here. Shader code contains only positive coefficients
    // as well as functions projecting directions onto SH space (for instance, project_sh9)
    if (l == m)
        return /*pow((T)-1, m) * */double_factorial(2 * m - 1) * pow(1 - x * x, m * (T)0.5);
    else if (l == m + 1)
        return x * (2 * m + 1) * legendre(m, m, x);
    else
        return (x * (2 * l - 1) * legendre(l - 1, m, x) - (l + m - 1) * legendre(l - 2, m, x)) / (l - m);
}

inline size_t factorial(int n)
{
    ASSERT(n >= 0, "Factorial is valid only for non-negative numbers");
    size_t res = 1;
    for (int i = 2; i <= n; ++i)
        res *= i;
    return res;
}

template <class T>
T K(int l, int m)
{
    return sqrt((T)(2 * l + 1) * factorial(l - ::abs(m)) / ((4 * mhe::pi) * factorial(l + ::abs(m))));
}

template <class T>
T cals_sh(int l, int m, T theta, T phi)
{
    if (m > 0) return sqrt(2.0f) * K<T>(l, m) * cos(m * phi) * legendre<T>(l, m, cos(theta));
    else if (m < 0) return sqrt(2.0f) * K<T>(l, m) * sin(-m * phi) * legendre<T>(l, -m, cos(theta));
    else return K<T>(l, 0) * legendre<T>(l, 0, cos(theta));
}

inline size_t sh_index(int l, int m)
{
    return l * (l + 1) + m;
}
}

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
                coeff[sh::sh_index(l, m)] = sh::cals_sh(l, m, theta, phi);
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
