#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <vector>
#include <cstring>	// for memcpy()
#include <cmath>
#include "math/math_utils.hpp"
#include "math/vector3d.hpp"

namespace mhe
{
	template <class T>
	class matrix
	{
		private:
			T m_[4][4];
		public:
			matrix()
			{
				load_identity();
			}

			matrix(const matrix& m)
			{
				*this = m;
			}

			matrix(const std::vector<T>& m)
			{
				set(m);
			}

			matrix(const T* m)
			{
				set(m);
			}

			//
			void load_identity()
			{
				m_[0][0] = 1; m_[0][1] = 0; m_[0][2] = 0; m_[0][3] = 0;
				m_[1][0] = 0; m_[1][1] = 1; m_[1][2] = 0; m_[1][3] = 0;
				m_[2][0] = 0; m_[2][1] = 0; m_[2][2] = 1; m_[2][3] = 0;
				m_[3][0] = 0; m_[3][1] = 0; m_[3][2] = 0; m_[3][3] = 1;
			}

			void set(const std::vector<T>& m)
			{
				if (m.size() < 16) return;
				set(&m[0]);
			}

			void set(const T* m)
			{
				// memcpy() are usually optimized
				memcpy(&m_, m, sizeof(T) << 4);
			}

			void set(T a11, T a12, T a13, T a14,
					 T a21, T a22, T a23, T a24,
					 T a31, T a32, T a33, T a34,
					 T a41, T a42, T a43, T a44)
			{
				m_[0][0] = a11; m_[0][1] = a12; m_[0][2] = a13; m_[0][3] = a14;
				m_[1][0] = a21; m_[1][1] = a22; m_[1][2] = a23; m_[1][3] = a24;
				m_[2][0] = a31; m_[2][1] = a32; m_[2][2] = a33; m_[2][3] = a34;
				m_[3][0] = a41; m_[3][1] = a42; m_[3][2] = a43; m_[3][3] = a44;
			}

			const T* get() const
			{
				return (T*)m_;
			}

			matrix& operator= (const matrix& m)
			{
				set(m.get());
				return *this;
			}

			matrix& operator*= (const matrix& m)
			{
				/*
				// compact variant
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						for (int k = 0; k < 4; ++k)
							m_[i][j] += m_[i][k] * m.m_[k][i];
					}
				}
				return *this;
				*/

				m_[0][0] = m_[0][0] * m.m_[0][0] + m_[0][1] * m.m_[1][0] +
						   m_[0][2] * m.m_[2][0] + m_[0][3] * m.m_[3][0];
				m_[0][1] = m_[0][0] * m.m_[0][1] + m_[0][1] * m.m_[1][1] +
						   m_[0][2] * m.m_[2][1] + m_[0][3] * m.m_[3][1];
				m_[0][2] = m_[0][0] * m.m_[0][2] + m_[0][1] * m.m_[1][2] +
						   m_[0][2] * m.m_[2][2] + m_[0][3] * m.m_[3][2];
				m_[0][3] = m_[0][0] * m.m_[0][3] + m_[0][1] * m.m_[1][3] +
						   m_[0][2] * m.m_[2][3] + m_[0][3] * m.m_[3][3];

				m_[1][0] = m_[1][0] * m.m_[0][0] + m_[1][1] * m.m_[1][0] +
						   m_[1][2] * m.m_[2][0] + m_[1][3] * m.m_[3][0];
				m_[1][1] = m_[1][0] * m.m_[0][1] + m_[1][1] * m.m_[1][1] +
						   m_[1][2] * m.m_[2][1] + m_[1][3] * m.m_[3][1];
				m_[1][2] = m_[1][0] * m.m_[0][2] + m_[1][1] * m.m_[1][2] +
						   m_[1][2] * m.m_[2][2] + m_[1][3] * m.m_[3][2];
				m_[1][3] = m_[1][0] * m.m_[0][3] + m_[1][1] * m.m_[1][3] +
						   m_[1][2] * m.m_[2][3] + m_[1][3] * m.m_[3][3];

				m_[2][0] = m_[2][0] * m.m_[0][0] + m_[2][1] * m.m_[1][0] +
						   m_[2][2] * m.m_[2][0] + m_[2][3] * m.m_[3][0];
				m_[2][1] = m_[2][0] * m.m_[0][1] + m_[2][1] * m.m_[1][1] +
						   m_[2][2] * m.m_[2][1] + m_[2][3] * m.m_[3][1];
				m_[2][2] = m_[2][0] * m.m_[0][2] + m_[2][1] * m.m_[1][2] +
						   m_[2][2] * m.m_[2][2] + m_[2][3] * m.m_[3][2];
				m_[2][3] = m_[2][0] * m.m_[0][3] + m_[2][1] * m.m_[1][3] +
						   m_[2][2] * m.m_[2][3] + m_[2][3] * m.m_[3][3];

				m_[3][0] = m_[3][0] * m.m_[0][0] + m_[3][1] * m.m_[1][0] +
						   m_[3][2] * m.m_[2][0] + m_[3][3] * m.m_[3][0];
				m_[3][1] = m_[3][0] * m.m_[0][1] + m_[3][1] * m.m_[1][1] +
						   m_[3][2] * m.m_[2][1] + m_[3][3] * m.m_[3][1];
				m_[3][2] = m_[3][0] * m.m_[0][2] + m_[3][1] * m.m_[1][2] +
						   m_[3][2] * m.m_[2][2] + m_[3][3] * m.m_[3][2];
				m_[3][3] = m_[3][0] * m.m_[0][3] + m_[3][1] * m.m_[1][3] +
						   m_[3][2] * m.m_[2][3] + m_[3][3] * m.m_[3][3];
				return *this;
			}

			matrix& operator*= (T v)
			{
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
						m_[i][j] *= v;
				}

				return *this;
			}

			matrix& operator+= (const matrix& m)
			{
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
						m_[i][j] += m.m_[i][j];
				}

				return *this;
			}

			matrix& operator-= (const matrix& m)
			{
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
						m_[i][j] -= m.m_[i][j];
				}

				return *this;
			}

			matrix operator+ (const matrix& m) const
			{
				return add(*this, m);
			}

			matrix operator- (const matrix& m) const
			{
				return subs(*this, m);
			}

			matrix operator* (const matrix& m) const
			{
				return mult(*this, m);
			}

			matrix operator* (T v) const
			{
				return mult(*this, v);
			}

			// projection and view manipulation
			void set_lookAt(const v3d& pos, const v3d& dir, const v3d& up)
			{
				v3d f(dir - pos);		// forward vector
				f.normalize();
				v3d s = cross(f, up);	// side vector
				s.normalize();
				// recompute up vector
				v3d u = cross(s, f);
				u.normalize();

				set(s.x(), u.x(), -f.x(), 0,
					s.y(), u.y(), -f.y(), 0,
					s.z(), u.z(), -f.z(), 0,
					0,     0,     0,      1);

				// make fast translate manipulation
				// note: next function are equals to:
				//
				// matrix tm;
				// tm.set_translate(-pos);
				// *this *= tm;
				//
				multTranslate(-pos);
			}

			void multTranslate(const v3d& v)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (v[i] == 0) continue;
					T t = v[i];
					m_[3][0] += t * m_[i][0];
					m_[3][1] += t * m_[i][1];
					m_[3][2] += t * m_[i][2];
					m_[3][3] += t * m_[i][3];
				}
			}

			void set_translate(const v3d& v)
			{
				set_translate(v.x(), v.y(), v.z());
			}

			void set_translate(T x, T y, T z)
			{
				set(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					x, y, z, 1);
			}

			void set_rotate_x(float angle)
			{
                angle = deg_to_rad(angle);
				float s = sin(angle);
				float c = cos(angle);
				set(1, 0, 0,  0,
                    0, c, -s, 0,
                    0, s, c,  0,
                    0, 0, 0,  1);
			}

			void set_rotate_y(float angle)
			{
			    angle = deg_to_rad(angle);
				float s = sin(angle);
				float c = cos(angle);
				set(c,  0, s,  0,
                    0,  1, 0,  0,
                    -s, 0, c,  0,
                    0,  0, 0,  1);
			}

			void set_rotate_z(float angle)
			{
			    angle = deg_to_rad(angle);
				float s = sin(angle);
				float c = cos(angle);
				set(c, -s, 0,  0,
                    s, c,  0,  0,
                    0, 0,  1,  0,
                    0, 0,  0,  1);
			}

			void set_scale(const v3d& s)
			{
			    set_scale(s.x(), s.y(), s.z());
			}

			void set_scale(T x, T y, T z)
			{
			    set(x, 0, 0, 0,
                    0, y, 0, 0,
                    0, 0, z, 0,
                    0, 0, 0, 1);
			}
			
			// function has been token from OSG
			void set_frustum(T left, T right, T bottom, T top, T z_near, T z_far)
			{
				T A = (right + left) / (right - left);
				T B = (top + bottom) / (top - bottom);
				T C = -(z_far + z_near) / (z_far - z_near);
				T D = -2.0 * z_far * z_near / (z_far - z_near);
				set(2.0 * z_near / (right - left), 0.0, 							0.0, 0.0,
					0.0, 						   2.0 * z_near / (top - bottom),   0.0, 0.0,
					A, 							   B, 								C, 	 -1.0,
					0.0, 						   0.0, 							D, 	 0.0);
			}
			
			void set_ortho(T left, T right, T bottom, T top, T z_near, T z_far)
			{
				T tx = -(right + left) / (right - left);
				T ty = -(top + bottom) / (top - bottom);
				T tz = -(z_far + z_near) / (z_far - z_near);
				set(2 / (right - left), 0, 					0, 					   0,
					0, 					2 / (top - bottom), 0, 					   0,
					0, 					0, 					-2 / (z_far - z_near), 0,
					tx, 				ty, 				tz, 				   1);
			}
			
			void set_perspective(T fov, T aspect_ratio, T z_near, T z_far)
			{
				float fov_tan = tan(deg_to_rad(fov * 0.5));
				T right = fov_tan * aspect_ratio * z_near;
				T left = -right;
				T top = fov_tan * z_near;
				T bottom = -top;
				set_frustum(left, right, bottom, top, z_near, z_far);
			}

			// create identity matrix
			static matrix identity()
			{
			    return matrix();
			}

			bool is_identity() const
			{
				return *this == identity();
			}
	};

	// helper functions
	template <class T>
	matrix<T> mult(const matrix<T>& m1, const matrix<T>& m2)
	{
		matrix<T> m(m1);
		m *= m2;
		return m;
	}

	template <class T>
	matrix<T> mult(const matrix<T>& m, T v)
	{
		matrix<T> m_(m);
		m_ *= v;
		return m_;
	}

	template <class T>
	matrix<T> add(const matrix<T>& m1, const matrix<T>& m2)
	{
		matrix<T> m(m1);
		m += m2;
		return m;
	}

	template <class T>
	matrix<T> subs(const matrix<T>& m1, const matrix<T>& m2)
	{
		matrix<T> m(m1);
		m -= m2;
		return m;
	}

	typedef matrix<float>  matrixf;
	typedef matrix<double> matrixd;
};

#endif
