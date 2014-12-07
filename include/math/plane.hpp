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

    T d() const
    {
        return d_;
    }
private:
	vector3<T> nrm_;
	T d_;
};

template <class T>
plane<T> abs(const plane<T>& p)
{
    const vector3<T>& nrm = p.normal();
    return plane<T>(fabs(nrm.x()), fabs(nrm.y()), fabs(nrm.z()), fabs(p.d()));
}

template <class T>
T dot(const plane<T>& p, const vector3<T>& v)
{
    return dot(p.normal(), v) + p.d();
}

typedef plane<float> planef;

}

#endif
