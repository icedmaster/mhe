#ifndef _VECTOR3D_HPP_
#define _VECTOR3D_HPP_

#include <cmath>

namespace mhe
{
    class v3d
    {
        private:
            float v_[3];
        public:
            v3d()
            {
                // create NULL vector
                v_[0] = v_[1] = v_[2] = 0.0;
            }

            v3d(float x, float y, float z)
            {
                v_[0] = x; v_[1] = y; v_[2] = z;
            }

            v3d(const v3d &v)
            {
                v_[0] = v.v_[0];
                v_[1] = v.v_[1];
                v_[2] = v.v_[2];
            }

            ~v3d() {} // do nothing

            inline void set(float v)
            {
                v_[0] = v_[1] = v_[2] = v;
            }

            inline void set(float x, float y, float z)
            {
                v_[0] = x; v_[1] = y; v_[2] = z;
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

            float length() const
            {
                return sqrt((v_[0] * v_[0]) + (v_[1] * v_[1]) + (v_[2] * v_[2]));
            }

            void normalize()
            {
                *this *= (1 / length());
            }

            const float* get() const
            {
                return v_;
            }

            inline bool operator== (const v3d& v) const
            {
                return ((v_[0] == v.v_[0]) && (v_[1] == v.v_[1]) && (v_[2] == v.v_[2])) ? true : false;
            }

            inline bool operator!= (const v3d& v)
            {
                return ((v_[0] != v.v_[0]) || (v_[1] != v.v_[1]) || (v_[2] != v.v_[2])) ? true : false;
            }

            inline v3d& operator= (const v3d& v)
            {
                v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2];
                return *this;
            }

            v3d& operator/= (const float n)
            {
                v_[0] /= n;
                v_[1] /= n;
                v_[2] /= n;
                return *this;
            }

            v3d& operator*= (const float n)
            {
                v_[0] *= n;
                v_[1] *= n;
                v_[2] *= n;
                return *this;
            }

            v3d operator- (const v3d& v)
            {
                v3d vv = *this;
                return vv -= v;
            }

            v3d operator+ (const v3d& v)
            {
                v3d vv = *this;
                return vv += v;
            }

            v3d& operator-= (const v3d& v)
            {
                v_[0] -= v.v_[0]; v_[1] -= v.v_[1]; v_[2] -= v.v_[2];
                return *this;
            }

            v3d& operator+= (const v3d& v)
            {
                v_[0] += v.v_[0]; v_[1] += v.v_[1]; v_[2] += v.v_[2];
                return *this;
            }

            v3d& operator^ (const v3d& v)
            {
                return cross(v);
            }

            v3d operator- () const
            {
                return v3d(-v_[0], -v_[1], -v_[2]);
            }

            float operator[] (int i) const
            {
                return v_[i];
            }

            v3d& cross(const v3d& v)
            {
                v_[0] = v_[1] * v.v_[2] - v_[2] * v.v_[1];
                v_[1] = v_[2] * v.v_[0] - v_[0] * v.v_[2];
                v_[2] = v_[0] * v.v_[1] - v_[1] * v.v_[0];
                return *this;
            }

            void rotate(float angle, const v3d& v)
            {
                rotate(angle, v.x(), v.y(), v.z());
            }

            void rotate(float angle, float x, float y, float z)
            {
                float cos_ = (float)cos(angle);
                float sin_ = (float)sin(angle);

                float nx, ny, nz;

                // Find the new x position for the new rotated point
                nx  = (cos_ + (1 - cos_) * x * x)       * v_[0];
                nx += ((1 - cos_) * x * y - z * sin_)	* v_[1];
                nx += ((1 - cos_) * x * z + y * sin_)	* v_[2];

                // Find the new y position for the new rotated point
                ny  = ((1 - cos_) * x * y + z * sin_)	* v_[0];
                ny += (cos_ + (1 - cos_) * y * y)		* v_[1];
                ny += ((1 - cos_) * y * z - x * sin_)	* v_[2];

                // Find the new z position for the new rotated point
                nz  = ((1 - cos_) * x * z - y * sin_)	* v_[0];
                nz += ((1 - cos_) * y * z + x * sin_)	* v_[1];
                nz += (cos_ + (1 - cos_) * z * z)		* v_[2];

                v_[0] = nx; v_[1] = ny; v_[2] = nz;
            }
    };

    inline v3d cross(const v3d& v1, const v3d& v2)
    {
        v3d v(v1.y() * v2.z() - v1.z() * v2.y(),
              v1.z() * v2.x() - v1.x() * v2.z(),
              v1.x() * v2.y() - v1.y() * v2.x());
        return v;
    }

    const v3d x_axis = v3d(1.0, 0.0, 0.0);
    const v3d y_axis = v3d(0.0, 1.0, 0.0);
    const v3d z_axis = v3d(0.0, 0.0, 1.0);
};

#endif
