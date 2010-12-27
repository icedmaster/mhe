#ifndef _VECTOR4D_HPP_
#define _VECTOR4D_HPP_

#include <cmath>

namespace mhe
{
    class v4d
    {
        private:
            float v_[4];
        public:
            v4d()
            {
                v_[0] = 0.0;  v_[1] = 0.0; v_[2] = 0.0; v_[3] = 0.0;
            }

            v4d(float x, float y, float z, float w)
            {
                v_[0] = x;  v_[1] = y; v_[2] = z; v_[3] = w;
            }

            v4d(const v4d &v)
            {
                v_[0] = v.v_[0];
                v_[1] = v.v_[1];
                v_[2] = v.v_[2];
                v_[3] = v.v_[3];
            }

            ~v4d() {} // do nothing

            inline void set(float v)
            {
                v_[0] = v_[1] = v_[2] = v_[3] = v;
            }

            inline void set(float x, float y, float z, float w)
            {
                v_[0] = x; v_[1] = y; v_[2] = z; v_[3] = w;
            }

			inline void set(const v3d& v, float w)
			{
				v_[0] = v.x(); v_[1] = v.y(); v_[2] = v.z(); v_[3] = w;
			}

            inline float x() const
            {
                return v_[0];
            }

            inline float y() const
            {
                return v_[1];
            }

            inline float z() const
            {
                return v_[2];
            }

            inline float w() const
            {
                return v_[3];
            }

            inline float r() const
            {
                return v_[0];
            }

            inline float g() const
            {
                return v_[1];
            }

            inline float b() const
            {
                return v_[2];
            }

            inline float a() const
            {
                return v_[3];
            }

			inline void set_x(float x)
			{
				v_[0] = x;
			}

			inline void set_y(float y)
			{
				v_[1] = y;
			}

			inline void set_z(float z)
			{
				v_[2] = z;
			}

			inline void set_w(float w)
			{
				v_[3] = w;
			}

            const float* get() const
            {
                return v_;
            }

            inline void as_v3d(v3d& v) const
            {
                v.set(v_[0], v_[1], v_[2]);
            }

            inline bool operator== (const v4d& v)
            {
                return ((v_[0] == v.v_[0]) && (v_[1] == v.v_[1]) && (v_[2] == v.v_[2]) && (v_[3] == v.v_[3])) ? true : false;
            }

            inline bool operator!= (const v4d& v)
            {
                return ((v_[0] != v.v_[0]) || (v_[1] != v.v_[1]) || (v_[2] != v.v_[2]) || (v_[3] != v.v_[3])) ? true : false;
            }

            inline v4d& operator= (const v4d& v)
            {
                v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2]; v_[3] = v.v_[3];
                return *this;
            }

			inline v4d& operator= (const v3d& v)
			{
                v_[0] = v.x(); v_[1] = v.y(); v_[2] = v.z();
                return *this;
			}

            v4d& operator/= (const float n)
            {
                v_[0] /= n;
                v_[1] /= n;
                v_[2] /= n;
                v_[3] /= n;
                return *this;
            }

            v4d& operator*= (const float n)
            {
                v_[0] *= n;
                v_[1] *= n;
                v_[2] *= n;
                v_[3] *= n;
                return *this;
            }

            v4d& operator- (const v4d& v)
            {
                v4d vv = *this;
                return vv -= v;
            }

            v4d& operator+ (const v4d& v)
            {
                v4d vv = *this;
                return vv += v;
            }

            v4d& operator-= (const v4d& v)
            {
                v_[0] -= v.v_[0]; v_[1] -= v.v_[1]; v_[2] -= v.v_[2]; v_[3] -= v.v_[3];
                return *this;
            }

            v4d& operator+= (const v4d& v)
            {
                v_[0] += v.v_[0]; v_[1] += v.v_[1]; v_[2] += v.v_[2]; v_[3] += v.v_[3];
                return *this;
            }
    };

    typedef v4d colorf;

    const colorf cfWhite = colorf(1.0, 1.0, 1.0, 0.0);
    const colorf cfBlack = colorf(0, 0, 0, 0);
    const colorf cfRed   = colorf(1.0, 0, 0, 0);
    const colorf cfGreen = colorf(0, 1.0, 0, 0);
    const colorf cfBlue  = colorf(0, 0, 1.0, 0);
};

#endif
