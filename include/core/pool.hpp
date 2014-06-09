#ifndef __POOL_HPP__
#define __POOL_HPP__

#include <iostream>
#include <cassert>

typedef unsigned short uint16_t;

namespace mhe {

template <class T, class I>
struct StructTypePolicy
{
	static void set(T& obj, I id)
	{
		obj.id = id;
	}

	static I get(const T& obj)
	{
		return obj.id;
	}
};

template <class T, class I>
struct ClassTypePolicy
{
	static void set(T& obj, I id)
	{
		obj.set_id(id);
	}

	static I get(const T& obj)
	{
		return obj.id();
	}
};

template < class T, size_t C, class I = uint16_t, class Policy = ClassTypePolicy<T, I> >
class Pool
{
	struct Index
	{
		I id;			// index in indexes array
		I index;	// index of element in element's array
		I next;
	};

	static const I invalid_id = static_cast<I>(-1);
public:
	Pool() :
		first_(0), last_(C - 1), size_(0)
	{
		for (size_t i = 0; i < C - 1; ++i)
		{
			indexes_[i].id = i;
			indexes_[i].next = i + 1;
			indexes_[i].index = invalid_id;
		}
	}

	I create() const
	{
		Index& ind = indexes_[first_];
		ind.index = size_++;
		T& obj = objects_[ind.index];
		Policy::set(obj, ind.id);
		first_ = ind.next;

		return ind.id;
	}

	T& get(I id) const
	{
		Index& ind = indexes_[id];
		assert(ind.index != invalid_id && Policy::get(objects_[ind.index]) == id);
		return objects_[ind.index];
	}

	void remove(I id)
	{
		Index& ind = indexes_[id];
		assert(ind.index != invalid_id);
		// swap with the last object
		T& obj = objects_[ind.index];
		obj = objects_[--size_];
		indexes_[Policy::get(obj)].index = ind.index;

		ind.index = invalid_id;
		indexes_[last_].next = id;
		last_ = id;
	}

	void debug()
	{
	}
private:
	mutable T objects_[C];
	mutable Index indexes_[C];
	mutable I first_;
	mutable I last_;
	mutable I size_;
};

}

#endif