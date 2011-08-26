#ifndef _VECTOR4D_HPP_
#define _VECTOR4D_HPP_

#include <cmath>
#include "math/vector3d.hpp"

namespace mhe
{
	template <class T>
    class vector4
    {
        private:
            T v_[4];
        public:
            vector4()
            {
                v_[0] = 0.0;  v_[1] = 0.0; v_[2] = 0.0; v_[3] = 0.0;
            }

            vector4(T x, T y, T z, T w)
            {
                v_[0] = x;  v_[1] = y; v_[2] = z; v_[3] = w;
            }

            vector4(const vector4 &v)
            {
                v_[0] = v.v_[0];
                v_[1] = v.v_[1];
                v_[2] = v.v_[2];
                v_[3] = v.v_[3];
            }

            ~vector4() {} // do nothing

            inline void set(T v)
            {
                v_[0] = v_[1] = v_[2] = v_[3] = v;
            }

            inline void set(T x, T y, T z, T w)
            {
                v_[0] = x; v_[1] = y; v_[2] = z; v_[3] = w;
            }

			inline void set(const vector3<T>& v, float w)
			{
				v_[0] = v.x(); v_[1] = v.y(); v_[2] = v.z(); v_[3] = w;
			}

            inline T x() const
            {
                return v_[0];
            }

            inline T y() const
            {
                return v_[1];
            }

            inline T z() const
            {
                return v_[2];
            }

            inline T w() const
            {
                return v_[3];
            }

            inline T r() const
            {
                return v_[0];
            }

            inline T g() const
            {
                return v_[1];
            }

            inline T b() const
            {
                return v_[2];
            }

            inline T a() const
            {
                return v_[3];
            }

			inline void set_x(T x)
			{
				v_[0] = x;
			}

			inline void set_y(T y)
			{
				v_[1] = y;
			}

			inline void set_z(T z)
			{
				v_[2] = z;
			}

			inline void set_w(T w)
			{
				v_[3] = w;
			}

            const T* get() const
            {
                return v_;
            }

			vector3<T> as_v3d() const
			{
				return vector3<T>(v_[0], v_[1], v_[2]);
			}

            inline bool operator== (const vector4& v)
            {
                return ((v_[0] == v.v_[0]) && (v_[1] == v.v_[1]) && (v_[2] == v.v_[2]) && (v_[3] == v.v_[3])) ? true : false;
            }

            inline bool operator!= (const vector4& v)
            {
                return ((v_[0] != v.v_[0]) || (v_[1] != v.v_[1]) || (v_[2] != v.v_[2]) || (v_[3] != v.v_[3])) ? true : false;
            }

            inline vector4& operator= (const vector4& v)
            {
                v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2]; v_[3] = v.v_[3];
                return *this;
            }

			inline vector4& operator= (const vector3<T>& v)
			{
                v_[0] = v.x(); v_[1] = v.y(); v_[2] = v.z();
                return *this;
			}

            vector4& operator/= (const T n)
            {
                v_[0] /= n;
                v_[1] /= n;
                v_[2] /= n;
                v_[3] /= n;
                return *this;
            }

            vector4& operator*= (const T n)
            {
                v_[0] *= n;
                v_[1] *= n;
                v_[2] *= n;
                v_[3] *= n;
                return *this;
            }

            vector4 operator* (T n) const
            {
                vector4 vv(*this);
                vv *= n;
                return vv;
            }

            vector4 operator- (const vector4& v) const
            {
                vector4 vv(*this);
                vv -= v;
                return vv;
            }

            vector4& operator+ (const vector4& v)
            {
                vector4 vv = *this;
                return vv += v;
            }

            vector4& operator-= (const vector4& v)
            {
                v_[0] -= v.v_[0]; v_[1] -= v.v_[1]; v_[2] -= v.v_[2]; v_[3] -= v.v_[3];
                return *this;
            }

            vector4& operator+= (const vector4& v)
            {
                v_[0] += v.v_[0]; v_[1] += v.v_[1]; v_[2] += v.v_[2]; v_[3] += v.v_[3];
                return *this;
            }

            float length2() const
            {
                return v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2] + v_[3] * v_[3];
            }
    };

	typedef vector4<float> v4d;
    typedef v4d colorf;

    const colorf cfWhite = colorf(1.0, 1.0, 1.0, 1.0);
    const colorf cfBlack = colorf(0, 0, 0, 1.0);
    const colorf cfRed   = colorf(1.0, 0, 0, 1.0);
    const colorf cfGreen = colorf(0, 1.0, 0, 1.0);
    const colorf cfBlue  = colorf(0, 0, 1.0, 1.0);
    const colorf cfYellow = colorf(1.0, 1.0, 0.0, 1.0);
};

#endif
