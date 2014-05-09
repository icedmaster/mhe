#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <vector>
#include <cstring>	// for memcpy()
#include <cmath>
#include <cassert>
#include <iostream>
#include "math_utils.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix3x3.hpp"

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

	void fill(T value)
	{
		for (int i = 0; i < 4; ++i)
		{
			m_[i][0] = m_[i][1] = m_[i][2] = m_[i][3] = value;
		}				
	}

	const T* get() const
	{
		return (T*)m_;
	}

    vector4<T> row(int r) const
    {
        return vector4<T>(m_[r][0], m_[r][1], m_[r][2], m_[r][3]);
    }

    vector4<T> column(int c) const
    {
        return vector4<T>(m_[0][c], m_[1][c], m_[2][c], m_[3][c]);
    }

    T element(int i, int j) const
    {
        return m_[i][j];
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

		T result[4][4];

		result[0][0] = m_[0][0] * m.m_[0][0] + m_[0][1] * m.m_[1][0] +
			m_[0][2] * m.m_[2][0] + m_[0][3] * m.m_[3][0];
		result[0][1] = m_[0][0] * m.m_[0][1] + m_[0][1] * m.m_[1][1] +
			m_[0][2] * m.m_[2][1] + m_[0][3] * m.m_[3][1];
		result[0][2] = m_[0][0] * m.m_[0][2] + m_[0][1] * m.m_[1][2] +
			m_[0][2] * m.m_[2][2] + m_[0][3] * m.m_[3][2];
		result[0][3] = m_[0][0] * m.m_[0][3] + m_[0][1] * m.m_[1][3] +
			m_[0][2] * m.m_[2][3] + m_[0][3] * m.m_[3][3];

		result[1][0] = m_[1][0] * m.m_[0][0] + m_[1][1] * m.m_[1][0] +
			m_[1][2] * m.m_[2][0] + m_[1][3] * m.m_[3][0];
		result[1][1] = m_[1][0] * m.m_[0][1] + m_[1][1] * m.m_[1][1] +
			m_[1][2] * m.m_[2][1] + m_[1][3] * m.m_[3][1];
		result[1][2] = m_[1][0] * m.m_[0][2] + m_[1][1] * m.m_[1][2] +
			m_[1][2] * m.m_[2][2] + m_[1][3] * m.m_[3][2];
		result[1][3] = m_[1][0] * m.m_[0][3] + m_[1][1] * m.m_[1][3] +
			m_[1][2] * m.m_[2][3] + m_[1][3] * m.m_[3][3];

		result[2][0] = m_[2][0] * m.m_[0][0] + m_[2][1] * m.m_[1][0] +
			m_[2][2] * m.m_[2][0] + m_[2][3] * m.m_[3][0];
		result[2][1] = m_[2][0] * m.m_[0][1] + m_[2][1] * m.m_[1][1] +
			m_[2][2] * m.m_[2][1] + m_[2][3] * m.m_[3][1];
		result[2][2] = m_[2][0] * m.m_[0][2] + m_[2][1] * m.m_[1][2] +
			m_[2][2] * m.m_[2][2] + m_[2][3] * m.m_[3][2];
		result[2][3] = m_[2][0] * m.m_[0][3] + m_[2][1] * m.m_[1][3] +
			m_[2][2] * m.m_[2][3] + m_[2][3] * m.m_[3][3];

		result[3][0] = m_[3][0] * m.m_[0][0] + m_[3][1] * m.m_[1][0] +
			m_[3][2] * m.m_[2][0] + m_[3][3] * m.m_[3][0];
		result[3][1] = m_[3][0] * m.m_[0][1] + m_[3][1] * m.m_[1][1] +
			m_[3][2] * m.m_[2][1] + m_[3][3] * m.m_[3][1];
		result[3][2] = m_[3][0] * m.m_[0][2] + m_[3][1] * m.m_[1][2] +
			m_[3][2] * m.m_[2][2] + m_[3][3] * m.m_[3][2];
		result[3][3] = m_[3][0] * m.m_[0][3] + m_[3][1] * m.m_[1][3] +
			m_[3][2] * m.m_[2][3] + m_[3][3] * m.m_[3][3];
		set(reinterpret_cast<const T*>(result));
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

	matrix& operator/= (T v)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				m_[i][j] /= v;
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

	matrix operator/ (T v) const
	{
		matrix m(*this);
		m /= v;
		return m;
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
		set(1, 0, 0,  0,
			0, c, s, 0,
			0, -s, c,  0,
			0, 0, 0,  1);
	}

	void set_rotate_y(float angle)
	{
		float s = sin(angle);
		float c = cos(angle);
		set(c,  0, s,  0,
			0,  1, 0,  0,
			-s, 0, c,  0,
			0,  0, 0,  1);
	}

	void set_rotate_z(float angle)
	{
		float s = sin(angle);
		float c = cos(angle);
		set(c, -s, 0,  0,
			s, c,  0,  0,
			0, 0,  1,  0,
			0, 0,  0,  1);
	}

	void set_rotate(float x_angle, float y_angle, float z_angle)
	{
		float sx = sin(x_angle), cx = cos(x_angle);
		float sy = sin(y_angle), cy = cos(y_angle);
		float sz = sin(z_angle), cz = cos(z_angle);
		// www.gamedev.ru/code/articles/faq_matrix_quat?page=4
		float ad = cx * sy;
		float bd = sx * sy;
		set(cy * cz, -bd * cz + cx * sz, ad * cz + sx * sz, 0,
			-cy * sz, bd * sz + cx * cz, -ad * sz + sx * cz, 0,
			-sy, -sx * cy, cx * cy, 0,
			0, 0, 0, 1);
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

    vector4<T> side_vector() const
    {
        return column(0);
    }

    vector4<T> up_vector() const
    {
        return column(1);
    }

    vector4<T> forward_vector() const
    {
        return -column(2);
    }

    float determinant() const
    {
        return
                m_[0][0] * m_[1][1] * m_[2][2] * m_[3][3] + m_[0][0] * m_[1][2] * m_[2][3] * m_[3][1] + m_[0][0] * m_[1][3] * m_[2][1] * m_[3][2] +
                m_[0][1] * m_[1][0] * m_[2][3] * m_[3][2] + m_[0][1] * m_[1][2] * m_[2][0] * m_[3][3] + m_[0][1] * m_[1][3] * m_[2][2] * m_[3][0] +
                m_[0][2] * m_[1][0] * m_[2][1] * m_[3][3] + m_[0][2] * m_[1][1] * m_[2][3] * m_[3][0] + m_[0][2] * m_[1][3] * m_[2][0] * m_[3][1] +
                m_[0][3] * m_[1][0] * m_[2][2] * m_[3][1] + m_[0][3] * m_[1][1] * m_[2][0] * m_[3][2] + m_[0][3] * m_[1][2] * m_[2][1] * m_[3][0] -
                m_[0][0] * m_[1][1] * m_[2][2] * m_[3][3] - m_[0][0] * m_[1][2] * m_[2][1] * m_[3][3] - m_[0][0] * m_[1][3] * m_[2][2] * m_[3][1] -
                m_[0][1] * m_[1][0] * m_[2][2] * m_[3][3] - m_[0][1] * m_[1][2] * m_[2][3] * m_[3][0] - m_[0][1] * m_[1][3] * m_[2][0] * m_[3][2] -
                m_[0][2] * m_[1][0] * m_[2][3] * m_[3][1] - m_[0][2] * m_[1][1] * m_[2][0] * m_[3][3] - m_[0][2] * m_[1][3] * m_[2][1] * m_[3][0] -
                m_[0][3] * m_[1][0] * m_[2][1] * m_[3][2] - m_[0][3] * m_[1][1] * m_[2][2] * m_[3][0] - m_[0][2] * m_[1][2] * m_[2][0] * m_[3][1];
    }

    void inverse()
    {
		T res[4][4];
		T m2233 = m_[2][2] * m_[3][3] - m_[2][3] * m_[3][2];
		T m2133 = m_[2][1] * m_[3][3] - m_[2][3] * m_[3][1];
		T m2132 = m_[2][1] * m_[3][2] - m_[2][2] * m_[3][1];
		T m1033 = m_[1][0] * m_[3][3] - m_[1][3] * m_[3][0];
		T m1032 = m_[1][0] * m_[3][2] - m_[1][2] * m_[3][0];
		T m2032 = m_[2][0] * m_[3][2] - m_[2][2] * m_[3][0];
		T m2031 = m_[2][0] * m_[3][1] - m_[2][1] * m_[3][0];
		T m2033 = m_[2][0] * m_[3][3] - m_[2][3] * m_[3][0];
		T m1233 = m_[1][2] * m_[3][3] - m_[1][3] * m_[3][2];
		T m1133 = m_[1][1] * m_[3][3] - m_[1][3] * m_[3][1];
		T m1132 = m_[1][1] * m_[3][2] - m_[1][2] * m_[3][1];
		T m1031 = m_[1][0] * m_[3][1] - m_[1][1] * m_[3][0];
		T m1223 = m_[1][2] * m_[2][3] - m_[1][3] * m_[2][2];
		T m1123 = m_[1][1] * m_[2][3] - m_[1][3] * m_[2][1];
		T m1122 = m_[1][1] * m_[2][2] - m_[1][2] * m_[2][1];
		T m1023 = m_[1][0] * m_[2][3] - m_[1][3] * m_[2][0];
		T m1022 = m_[1][0] * m_[2][2] - m_[1][2] * m_[2][0];
		T m1021 = m_[1][0] * m_[2][1] - m_[1][1] * m_[2][0];

		res[0][0] = m_[1][1] * m2233 - m_[1][2] * m2133 + m_[1][3] * m2132;
		res[1][0] = -(m_[1][0] * m2233 - m_[1][2] * m2033 + m_[1][3] * m2032);
		res[2][0] = m_[1][0] * m2133 - m_[1][1] * m2033 + m_[1][3] * m2031;
		res[3][0] = -(m_[1][0] * m2132 - m_[1][1] * m2032 + m_[1][2] * m2031);
		res[0][1] = -(m_[0][1] * m2233 - m_[0][2] * m2133 + m_[0][3] * m2132);
		res[1][1] = m_[0][0] * m2233 - m_[0][2] * m2033 + m_[0][3] * m2032;
		res[2][1] = -(m_[0][0] * m2133 - m_[0][1] * m2033 + m_[0][3] * m2031);
		res[3][1] = m_[0][0] * m2132 - m_[0][1] * m2032 + m_[0][2] * m2031;
		res[0][2] = m_[0][1] * m1233 - m_[0][2] * m1133 + m_[0][3] * m1132;
		res[1][2] = -(m_[0][0] * m1233 - m_[0][2] * m1033 + m_[0][3] * m1032);
		res[2][2] = m_[0][0] * m1133 - m_[0][1] * m1033 + m_[0][3] * m1031;
		res[3][2] = -(m_[0][0] * m1132 - m_[0][1] * m1032 + m_[0][2] * m1031);
		res[0][3] = -(m_[0][1] * m1223 - m_[0][2] * m1123 + m_[0][3] * m1122);
		res[1][3] = m_[0][0] * m1223 - m_[0][2] * m1023 + m_[0][3] * m1022;
		res[2][3] = -(m_[0][0] * m1123 - m_[0][1] * m1023 + m_[0][3] * m1021);
		res[3][3] = m_[0][0] * m1122 - m_[0][1] * m1022 + m_[0][2] * m1021;

		T det = m_[0][0] * res[0][0] + m_[0][1] * res[1][0] + m_[0][2] * res[2][0] + m_[0][3] * res[3][0];
		std::cout << det << std::endl;
		if (det == 0)
		{
			assert(0);
			load_identity();
			return;
		}
		float inv_det = 1.0f / det;
		set(reinterpret_cast<const T*>(res));
		*this *= inv_det;
    }

	matrix<T> inverted() const
	{
		matrix<T> copy(*this);
		copy.inverse();
		return copy;
	}

	void transpose()
	{
		T t = m_[0][1];
		m_[0][1] = m_[1][0];
		m_[1][0] = t;

		t = m_[0][2];
		m_[0][2] = m_[2][0];
		m_[2][0] = t;

		t = m_[0][3];
		m_[0][3] = m_[3][0];
		m_[3][0] = t;

		t = m_[1][2];
		m_[1][2] = m_[2][1];
		m_[2][1] = t;

		t = m_[1][3];
		m_[1][3] = m_[3][1];
		m_[3][1] = t;

		t = m_[2][3];
		m_[2][3] = m_[3][2];
		m_[3][2] = t;
	}

	matrix<T> transposed() const
	{
		matrix<T> copy(*this);
		copy.transpose();
		return copy;
	}

    matrix3x3<T> as_matrix3x3() const
    {
        return matrix3x3<T>(m_[0][0], m_[0][1], m_[0][2],
                m_[1][0], m_[1][1], m_[1][2],
                m_[2][0], m_[2][1], m_[2][2]);
    }

	// create identity matrix
	static matrix identity()
	{
		return matrix();
	}

	static matrix translation_matrix(const v3d& v)
	{
		matrix m;
		m.set_translate(v);
		return m;
	}

	static matrix translation_matrix(T x, T y, T z)
	{
		matrix m;
		m.set_translate(x, y, z);
		return m;
	}

	static matrix scaling_matrix(const v3d& v)
	{
		matrix m;
		m.set_scale(v);
		return m;
	}

	static matrix rotation_matrix(float x_angle, float y_angle, float z_angle)
	{
		matrix m;
		m.set_rotate(x_angle, y_angle, z_angle);
		return m;
	}

	static matrix empty_matrix()
	{
		matrix m;
		m.fill(0);
		return m;
	}

	bool is_identity() const
	{
		return *this == identity();
	}

	// operations with vectors
	vector3<T> premult(const vector3<T>& v) const
	{
		return vector3<T>(m_[0][0] * v.x() + m_[1][0] * v.y() + m_[2][0] * v.z() + m_[3][0],
						  m_[0][1] * v.x() + m_[1][1] * v.y() + m_[2][1] * v.z() + m_[3][1],
						  m_[0][2] * v.x() + m_[1][2] * v.y() + m_[2][2] * v.z() + m_[3][2]);
	}

	vector3<T> postmult(const vector3<T>& v) const
	{
		return vector3<T>(m_[0][0] * v.x() + m_[0][1] * v.y() + m_[0][2] * v.z() + m_[0][3],
						  m_[1][0] * v.x() + m_[1][1] * v.y() + m_[1][2] * v.z() + m_[1][3],
						  m_[2][0] * v.x() + m_[2][1] * v.y() + m_[2][2] * v.z() + m_[2][3]);		
	}
};

// helper functions
template <class T>
inline matrix<T> mult(const matrix<T>& m1, const matrix<T>& m2)
{
	matrix<T> m(m1);
	m *= m2;
	return m;
}

template <class T>
inline matrix<T> mult(const matrix<T>& m, T v)
{
	matrix<T> m_(m);
	m_ *= v;
	return m_;
}

template <class T>
inline matrix<T> add(const matrix<T>& m1, const matrix<T>& m2)
{
	matrix<T> m(m1);
	m += m2;
	return m;
}

template <class T>
inline matrix<T> subs(const matrix<T>& m1, const matrix<T>& m2)
{
	matrix<T> m(m1);
	m -= m2;
	return m;
}

template <class T>
inline vector3<T> operator* (const vector3<T>& v, const matrix<T>& m)
{
	return m.premult(v);
}

template <class T>
inline vector3<T> operator* (const matrix<T>& m, const vector3<T>& v)
{
	return m.postmult(v);
}

template <class T>
inline std::ostream& operator<< (std::ostream& stream, const matrix<T>& m)
{
	stream << "{";
	for (int i = 0; i < 4; ++i)
	{
		stream << "{";
		for (int j = 0; j < 4; ++j)
		{
			stream << m.get()[i * 4 + j];
			if (j != 3) stream << ",";
		}
		stream << "}";
		if (i != 3)	stream << ",";
	}
	stream << "}";
	return stream;
}

typedef matrix<float>  matrixf;
typedef matrix<double> matrixd;
typedef matrix<float> mat4x4;

}

#endif
