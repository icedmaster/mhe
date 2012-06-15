#ifndef _QUAT_HPP_
#define _QUAT_HPP_

#include "vector3.hpp"
//#include "vector4d.hpp"

namespace mhe
{
	template <class T>
	class matrix;

	template<class T>
	class quat
	{
		private:
			v3d v_;
			T w_;
		public:
			// zero rotation quaternion
			quat() :
				w_(1.0)
			{}
			
			quat(T x, T y, T z, T w) :
				v_(x, y, z),
				w_(w)
			{}
			
			quat(const v3d& v, T w) :
				v_(v),
				w_(w)
			{}
			
			quat(const quat& q) :
				v_(q.v_),
				w_(q.w_)
			{}
			
			void set(T x, T y, T z, T w)
			{
				v_.set(x, y, z);
				w_ = w;
			}
			
			void set(const v3d& v, T w)
			{
				v_ = v;
				w_ = w;
			}
			
			T x() const
			{
				return v_.x();
			}
			
			T y() const
			{
				return v_.y();
			}

			T z() const
			{
				return v_.z();
			}
			
			const v3d& v() const
			{
				return v_;
			}
			
			T w() const
			{
				return w_;
			}
						
			T length() const
			{
				return sqrt(v_.length() + w_ * w_);
			}
			
			T norm() const
			{
			    // TODO:
				return v_.length() + w_ * w_;
			}			
			
			quat& conj()
			{
				//v_ = -v_;
				return *this;
			}
			
			void normalize()
			{
				*this /= length();
			}
			
			// operators
			quat& operator= (const quat& q)
			{
				v_ = q.v_;
				w_ = q.w_;
				return *this;
			}
			
			quat& operator+= (const quat& q)
			{
				v_ += q.v_;
				w_ += q.w_;
				return *this;
			}
			
			quat& operator-= (const quat& q)
			{
				v_ -= q.v_;
				w_ -= q.w_;
				return *this;
			}
			
			quat& operator*= (const quat& q)
			{
				w_ = w_ * q.w_ - v_ * q.v_;
				v_ = cross(v_, q.v_) + w_ * q.v_ + q.w_ * v_;
				return *this;
			}
			
			quat& operator*= (T v)
			{
				v_ *= v;
				w_ *= v;
				return *this;
			}
			
			quat& operator/= (T v)
			{
				v_ /= v;
				w_ /= v;
				return *this;
			}
			
			quat operator+ (const quat& q) const
			{
				return q_add(*this, q);
			}
			
			quat operator- (const quat& q) const
			{
				return q_subs(*this, q);
			}
			
			quat operator* (const quat& q) const
			{
				return q_mult(*this, q);
			}
			
			quat operator* (T v) const
			{
				return q_mult(*this, v);
			}
			
			quat operator/ (T v) const
			{
				return q_div(*this, v);
			}
			
			// representation
			//v4d as_v4d() const
			//{
			//	return v4d(v_, w_);
			//}
			
			// matrix
			matrix<T> to()
			{
				return matrix<T>();
			}
	};
	
	// helper functions
	template <class T>
	inline quat<T> q_add(const quat<T>& q1, const quat<T>& q2)
	{
		quat<T> q(q1);
		q += q2;
		return q;
	}
	
	template <class T>
	inline quat<T> q_subs(const quat<T>& q1, const quat<T>& q2)
	{
		quat<T> q(q1);
		q -= q2;
		return q;
	}
	
	template <class T>
	inline quat<T> q_mult(const quat<T>& q1, const quat<T>& q2)
	{
		quat<T> q(q1);
		q *= q2;
		return q;
	}
	
	template <class T>
	inline quat<T> q_mult(const quat<T>& q, T v)
	{
		quat<T> q_(q);
		q_ *= v;
		return q_;
	}
	
	template <class T>
	inline quat<T> q_div(const quat<T>& q, T v)
	{
		quat<T> q_(q);
		q_ /= v;
		return q_;
	}
	
	template <class T>
	inline quat<T> q_conj(const quat<T>& q)
	{
		quat<T> q_(q);
		q_.conj();
		return q_;
	}		
	
	// typedefs
	typedef quat<float> quatf;
};

#endif
