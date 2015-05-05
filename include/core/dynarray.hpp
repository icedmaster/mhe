#ifndef __DYNARRAY_HPP__
#define __DYNARRAY_HPP__

#include <cstdlib>
#include <cassert>
#include "allocator.hpp"

namespace mhe {

template <class T, size_t count>
class dynarray
{
public:
public:
	typedef dynarray<T, count> this_type;
	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;
public:
	dynarray(allocator* alloc = default_allocator()) :
		elements_(new (alloc) T[count]),
		allocator_(alloc)
	{}

	dynarray(const this_type& other, allocator* alloc = default_allocator()) :
		elements_(new (alloc) T[count]),
		allocator_(alloc)
	{
		swap(other);
	}

	dynarray(const T& value, allocator* alloc = default_allocator()) :
		elements_(new (alloc) T[count]),
		allocator_(alloc)
	{
		fill(value);
	}

	~dynarray()
	{
		delete[](allocator_, elements_);
	}

	// accessors
	iterator begin()
	{
		return elements_;
	}

	const_iterator begin() const
	{
		return elements_;
	}

	iterator end()
	{
		return elements_ + count;
	}

	const_iterator end() const
	{
		return elements_ + count;
	}

	T& operator[] (size_t index)
	{
		return elements_[index];
	}

	T operator[] (size_t index) const
	{
		return elements_[index];
	}

	T& at(size_t index)
	{
		assert(index < count);
		return elements_[index];
	}

	T at(size_t index) const
	{
		assert(index < count);
		return elements_[index];
	}

	T& front()
	{
		return elements_[0];
	}

	T& back()
	{
		return elements_[count - 1];
	}

	const T* data() const
	{
		return elements_;
	}

	size_t size() const
	{
		return count;
	}

	void fill(const T& value)
	{
		for (size_t i = 0; i < count; ++i)
			elements_[i] = value;
	}

	void swap(const this_type& other)
	{
		for (size_t i = 0; i < count; ++i)
			elements_[i] = other[i];
	}
private:
	T* elements_;
	allocator* allocator_;
};

}

#endif
