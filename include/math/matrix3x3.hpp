#ifndef __MATRIX3X3_HPP__
#define __MATRIX3X3_HPP__

#

namespace mhe {

template <class T>
class matrix3x3
{
public:
    matrix3x3()
    {
        load_identity();
    }

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
        m_[0][0] = a11; m_[0][1] = a12; m_[0][2] = a13;
        m_[1][0] = a21; m_[1][1] = a22; m_[1][2] = a23;
        m_[2][0] = a31; m_[2][1] = a32; m_[2][2] = a33;
    }

    T operator()(size_t i, size_t j)
    {
        ASSERT(i < 3 && j < 3, "Invalid index");
        return m_[i][j];
    }

    const T* get() const
    {
        return reinterpret_cast<const T*>(m_);
    }

    T* data()
    {
        return reinterpret_cast<const T*>(m_);
    }

    void transpose()
    {
        T t = m_[0][1];
        m_[0][1] = m_[1][0];
        m_[1][0] = t;

        t = m_[0][2];
        m_[0][2] = m_[2][0];
        m_[2][0] = t;

        t = m_[1][2];
        m_[1][2] = m_[2][1];
        m_[2][1] = t;
    }

    matrix3x3 transposed() const
    {
        matrix3x3 m = *this;
        m.transpose();
        return m;
    }

    void load_identity()
    {
        m_[0][0] = (T)1; m_[0][1] = (T)0; m_[0][2] = (T)0;
        m_[1][0] = (T)0; m_[1][1] = (T)1; m_[1][2] = (T)0;
        m_[2][0] = (T)0; m_[2][1] = (T)0; m_[2][2] = (T)1;
    }

    matrix3x3& operator= (const matrix3x3& other)
    {
        memcpy(m_, other.m_, sizeof(m_));
        return *this;
    }
private:
    float m_[3][3];
};

typedef matrix3x3<float> mat3x3;

}

#endif
