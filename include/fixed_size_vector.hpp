#ifndef __FIXED_SIZE_VECTOR_HPP__
#define __FIXED_SIZE_VECTOR_HPP__

#include <iostream>
#include <cstring>
#include <sstream>
#include <cassert>

namespace mhe {

template <class T, size_t count>
class fixed_size_vector
{
public:
	typedef fixed_size_vector<T, count> this_type;
	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;
public:
	fixed_size_vector() :
		begin_(elements_), size_(0), capacity_(count)
	{}

	fixed_size_vector(const this_type& v) :
		begin_(elements_), size_(0), capacity_(count)
	{
		insert(end(), v.begin(), v.end());
	}

	fixed_size_vector(size_t size) :
		begin_(elements_), capacity_(count)
	{
		resize(size);
	}

	~fixed_size_vector()
	{
		if (begin_ != elements_)
			delete [] begin_;
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
		return begin_[index];
	}

	T operator[] (size_t index) const
	{
		return begin_[index];
	}

	T& at(size_t index)
	{
		assert(index < size_);
		return begin_[index];
	}

	T at(size_t index) const
	{
		assert(index < size_);
		return begin_[index];
	}

	T& front()
	{
		return begin_[0];
	}

	T& back()
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
		size_t count = 0;
		for (; first != last; ++first)
			insert_impl(first_index + count++, *first);
		return begin_ + count;
	}

	iterator insert(const_iterator position, const T& value)
	{
		return insert_impl(position - begin(), value);
	}

	void push_back(const T& value)
	{
		insert(end(), value);
	}

	void resize(size_t new_size)
	{
		if (new_size > capacity_)
			reallocate_vector(new_size);
		size_ = new_size;
	}

	// erasing

	void clear()
	{
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
			reallocate_vector(capacity_ * 2);
		if (index != size_)
			::memmove(begin_ + index + 1, begin_ + index, (size_ - index) * sizeof(T));
		begin_[index] = value;
		++size_;
		return begin_ + index;
	}

	void reallocate_vector(size_t new_capacity)
	{
		size_t prev_capacity = capacity_;
		T* copy = new T[prev_capacity];
		::memmove(copy, begin_, size_ * sizeof(T));
		if (begin_ != elements_)
			delete [] begin_;
		capacity_ = new_capacity;
		begin_ = new T[capacity_];
		::memmove(begin_, copy, size_ * sizeof(T));
		delete [] copy;
	}

	iterator erase_impl(size_t index, size_t erased_count)
	{
		if (index != (size_ - erased_count))
			::memmove(begin_ + index, begin_ + index + erased_count, size_ - index - erased_count);
		size_ -= erased_count;
		return begin_ + index;
	}

	T elements_[count];
	T* begin_;
	size_t size_;
	size_t capacity_;
};

template <class T, size_t count>
inline std::ostream& operator<< (std::ostream& stream, const fixed_size_vector<T, count>& vector)
{
	std::stringstream ss;
	ss << "{";
	for (size_t i = 0; i < vector.size(); ++i)
		ss << vector[i] << ", ";
	ss << "}";
	stream << ss.str();
	return stream;
}

}

#endif
