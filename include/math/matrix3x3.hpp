#ifndef __MATRIX3X3_HPP__
#define __MATRIX3X3_HPP__

namespace mhe {

template <class T>
class matrix3x3
{
public:
	matrix3x3(T a11, T a12, T a13,
			  T a21, T a22, T a23,
			  T a31, T a32, T a33)
	{
		set(a11, a12, a13,
			a21, a22, a23,
			a31, a32, a33);
	}

	void set(T a11, T a12, T a13,
			 T a21, T a22, T a23,
			 T a31, T a32, T a33)
	{
		m[0][0] = a11; m[0][1] = a12; m[0][2] = a13;
		m[1][0] = a21; m[1][1] = a22; m[1][2] = a23;
		m[2][0] = a31; m[2][1] = a32; m[2][2] = a33;
	}

	const T* get() const
	{
        return reinterpret_cast<const T*>(m);
	}
private:
	float m[3][3];
};

typedef matrix3x3<float> mat3x3;

}

#endif
