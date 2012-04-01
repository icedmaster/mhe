#ifndef _VECTOR3D_HPP_
#define _VECTOR3D_HPP_

#include <cmath>

namespace mhe
{
	template <class T>
    class vector3
    {
        private:
            T v_[3];
        public:
            vector3()
            {
                // create NULL vector
                v_[0] = v_[1] = v_[2] = 0;
            }

            vector3(T x, T y, T z)
            {
                v_[0] = x; v_[1] = y; v_[2] = z;
            }

            vector3(const vector3 &v)
            {
                v_[0] = v.v_[0];
                v_[1] = v.v_[1];
                v_[2] = v.v_[2];
            }

            ~vector3() {} // do nothing

            inline void set(T v)
            {
                v_[0] = v_[1] = v_[2] = v;
            }

            inline void set(T x, T y, T z)
            {
                v_[0] = x; v_[1] = y; v_[2] = z;
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

            float length() const
            {
                return sqrt((v_[0] * v_[0]) + (v_[1] * v_[1]) + (v_[2] * v_[2]));
            }

            void normalize()
            {
                *this *= (1 / length());
            }

            const T* get() const
            {
                return v_;
            }

            inline bool operator== (const vector3& v) const
            {
                return ((v_[0] == v.v_[0]) && (v_[1] == v.v_[1]) && (v_[2] == v.v_[2])) ? true : false;
            }

            inline bool operator!= (const vector3& v)
            {
                return ((v_[0] != v.v_[0]) || (v_[1] != v.v_[1]) || (v_[2] != v.v_[2])) ? true : false;
            }

            inline vector3& operator= (const vector3& v)
            {
                v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2];
                return *this;
            }

            vector3& operator/= (const T n)
            {
                v_[0] /= n;
                v_[1] /= n;
                v_[2] /= n;
                return *this;
            }

			vector3 operator/ (const T n) const
			{
				vector3 vv = *this;
				return vv /= n;
			}

            vector3& operator*= (const T n)
            {
                v_[0] *= n;
                v_[1] *= n;
                v_[2] *= n;
                return *this;
            }

			vector3& operator*= (const vector3& v)
			{
				v_[0] *= v.v_[0];
				v_[1] *= v.v_[1];
				v_[2] *= v.v_[2];
				return *this;
			}

            vector3 operator- (const vector3& v) const
            {
                vector3 vv = *this;
                return vv -= v;
            }

            vector3 operator- () const
            {
                return vector3(-v_[0], -v_[1], -v_[2]);
            }

            vector3 operator+ (const vector3& v) const
            {
                vector3 vv = *this;
                return vv += v;
            }

            vector3& operator-= (const vector3& v)
            {
                v_[0] -= v.v_[0]; v_[1] -= v.v_[1]; v_[2] -= v.v_[2];
                return *this;
            }

            vector3& operator+= (const vector3& v)
            {
                v_[0] += v.v_[0]; v_[1] += v.v_[1]; v_[2] += v.v_[2];
                return *this;
            }

            vector3 operator^ (const vector3& v)
            {
                return cross(v);
            }

            T operator[] (int i) const
            {
                return v_[i];
            }

            vector3& cross(const vector3& v)
            {
                v_[0] = v_[1] * v.v_[2] - v_[2] * v.v_[1];
                v_[1] = v_[2] * v.v_[0] - v_[0] * v.v_[2];
                v_[2] = v_[0] * v.v_[1] - v_[1] * v.v_[0];
                return *this;
            }
    };

	template <class T>
    inline vector3<T> cross(const vector3<T>& v1, const vector3<T>& v2)
    {
        vector3<T> v(v1.y() * v2.z() - v1.z() * v2.y(),
				     v1.z() * v2.x() - v1.x() * v2.z(),
                     v1.x() * v2.y() - v1.y() * v2.x());
        return v;
    }
	
	typedef vector3<float> v3d;

    const v3d x_axis = v3d(1.0, 0.0, 0.0);
    const v3d y_axis = v3d(0.0, 1.0, 0.0);
    const v3d z_axis = v3d(0.0, 0.0, 1.0);
};

#endif
