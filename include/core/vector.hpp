#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include "compiler.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"

#ifdef MHE_VS
// TODO: need to check out this warning later
#pragma warning( disable: 4996 )	// copy may be unsafe
#endif

#include <iostream>
#include <cstring>
#include <sstream>
#include <cassert>

namespace mhe {

template <class T, size_t S = 0>
class vector
{
public:
	typedef vector<T> this_type;
	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;
public:
	vector(allocator* alloc = default_allocator()) :
        begin_(nullptr), size_(0), capacity_(0), allocator_(alloc)
	{
		if (S != 0)
			resize(S);
	}

	vector(const this_type& v, allocator* alloc = default_allocator()) :
        begin_(nullptr), size_(0), capacity_(0), allocator_(alloc)
	{
		insert(end(), v.begin(), v.end());
	}

	vector(size_t size, allocator* alloc = default_allocator()) :
        begin_(nullptr), size_(0), capacity_(0), allocator_(alloc)
	{		
		resize(size);
	}

	template <class InputIterator>
	vector(InputIterator first, InputIterator last, allocator* alloc = default_allocator()) :
		begin_(nullptr), size_(0), capacity_(0)
	{
		insert(end(), first, last);
	}

	~vector()
	{
		clear();
		free(begin_);
	}

	void set_allocator(allocator* alloc)
	{
		if (capacity_ != 0)
		{
			ASSERT(0, "Can't set allocator - some data has been allocated already using different allocator");
			return;
		}
		allocator_ = alloc;
	}

	// accessors
	iterator begin()
	{
		return begin_;
	}

	const_iterator begin() const
	{
		return begin_;
	}

	iterator end()
	{
		return begin_ + size_;
	}

	const_iterator end() const
	{
		return begin_ + size_;
	}

	T& operator[] (size_t index)
	{
		ASSERT(index < size_, "Invalid index " << index);
		return begin_[index];
	}

	const T& operator[] (size_t index) const
	{
		ASSERT(index < size_, "Invalid index " << index);
		return begin_[index];
	}

	T& at(size_t index)
	{
		ASSERT(index < size_, "Invalid index " << index);
		return begin_[index];
	}

	T at(size_t index) const
	{
		ASSERT(index < size_, "Invalid index " << index);
		return begin_[index];
	}

	T& front()
	{
		return begin_[0];
	}

	const T& front() const
	{
		return begin_[0];
	}

	T& back()
	{
		return begin_[size_ - 1];
	}

	const T& back() const
	{
		return begin_[size_ - 1];
	}

	const T* data() const
	{
		return begin_;
	}

	size_t size() const
	{
		return size_;
	}

	size_t capacity() const
	{
		return capacity_;
	}

	bool empty() const
	{
		return !size_;
	}

	// inserting elements
	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last)
	{
		size_t first_index = position - begin();
		size_t inserted_count = 0;
		for (; first != last; ++first)
			insert_impl(first_index + inserted_count++, *first);
		return begin_ + inserted_count;
	}

	iterator insert(const_iterator position, const T& value)
	{
		return insert_impl(position - begin(), value);
	}

	T& push_back(const T& value)
	{
		insert(end(), value);
		return back();
	}

	void append(const T* values, size_t size)
	{
		insert(end(), values, values + size);
	}

	void resize(size_t new_size)
	{
		if (new_size > capacity_)
			reallocate_vector(new_size);
		size_ = new_size;
	}

	void reserve(size_t capacity)
	{
		if (capacity > capacity_)
			reallocate_vector(capacity);
	}

	void fill(const T& value)
	{
		for (size_t i = 0; i < capacity_; ++i)
			begin_[i] = value;
	}

	// erasing

	void clear()
	{
		for (size_t i = 0; i < size_; ++i)
			begin_[i].~T();
		size_ = 0;
	}	

	iterator erase(const_iterator position)
	{
		return erase_impl(position - begin(), 1);
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		size_t index = first - begin();
		size_t erased_count = 0;
		for (; first != last; ++first) ++erased_count;
		return erase_impl(index, erased_count);
	}

	// operators
	this_type& operator= (const this_type& other)
	{
		clear();
		insert(end(), other.begin(), other.end());
		return *this;
	}

	bool operator== (const this_type& other) const
	{
		if (size_ != other.size_) return false;
		for (size_t i = 0; i < size_; ++i)
		{
			if (begin_[i] != other.begin_[i]) return false;
		}
		return true;
	}

	bool operator!= (const this_type& other) const
	{
		return !(*this == other);
	}
private:
	iterator insert_impl(size_t index, const T& value)
	{
		if ((size_ + 1) > capacity_)
			reallocate_vector(static_cast<size_t>(capacity_ == 0 ? 1 : capacity_ * 2.0f));
		if (index != size_)
			mhe::copy(begin_ + index, begin_ + size_ - index, begin_ + index + 1);
		begin_[index] = value;
		++size_;
		return begin_ + index;
	}

	void reallocate_vector(size_t new_capacity)
	{
		T* copy = begin_;
		capacity_ = new_capacity;
		begin_ = allocate(capacity_);
		mhe::copy(copy, copy + size_, begin_);
		free(copy);
	}

	iterator erase_impl(size_t index, size_t erased_count)
	{
		if (index != (size_ - erased_count))
			mhe::copy(begin_ + index + erased_count, begin_ + size_, begin_ + index);
		size_ -= erased_count;
		return begin_ + index;
	}

	T* allocate(size_t n)
	{
		return create_array<T>(n, allocator_);
	}

	void free(T* ptr)
	{
		destroy_array(ptr, allocator_);
	}

	T* begin_;
	size_t size_;
	size_t capacity_;
	allocator* allocator_;
};

template <class T>
inline std::ostream& operator<< (std::ostream& stream, const vector<T>& v)
{
	std::stringstream ss;
	ss << "{";
	for (size_t i = 0; i < v.size(); ++i)
		ss << v[i] << ", ";
	ss << "}";
	stream << ss.str();
	return stream;
}

}

#endif
