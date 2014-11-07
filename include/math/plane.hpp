#ifndef __PLANE_HPP__
#define __PLANE_HPP__

#include "vector3.hpp"

namespace mhe {

template <class T>
class plane
{
public:
	plane() {}
	
	template <class U>
	plane(U a, U b, U c, U d) :
		nrm_(a, b, c), d_(d)
	{}

	template <class U>
	plane(const vector3<U>& nrm, U d) :
		nrm_(nrm), d_(d)
	{}

	template <class U>
	void set(U nx, U ny, U nz, U d)
	{
		nrm_.set(nx, ny, nz);
		d_ = d;
	}

	const vector3<T>& normal() const
	{
		return nrm_;
	}
private:
	vector3<T> nrm_;
	T d_;
};

typedef plane<float> planef;

}

#endif
