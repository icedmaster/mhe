#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <vector>
#include <cstring>	// for memcpy()
#include <cmath>
#include "vector3d.hpp"

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
				float s = sin(angle);
				float c = cos(angle);
			}

			void set_rotate_y(float angle)
			{
				float s = sin(angle);
				float c = cos(angle);
			}

			void set_rotate_z(float angle)
			{
				float s = sin(angle);
				float c = cos(angle);
			}
	};

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
