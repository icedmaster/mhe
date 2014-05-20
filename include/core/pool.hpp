#ifndef __POOL_HPP__
#define __POOL_HPP__

#include "fixed_size_vector.hpp"

namespace mhe {

template <class T, size_t C = 128>
class Pool
{
public:
	T& get()
	{
		return pool_[index_++];
	}
private:
	fixed_size_vector<T, C> pool_;
	size_t index_;
};

}

#endif
